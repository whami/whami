<?php

/**
 * Convert a "bounding box" into a "geographic circle"
 *
 * "bounding box" denoted by 2 geopoints
 * "geographic circle" a geographic point and a radius that nears that "bounding box"
 *
 *
 * @param double $north
 *   The longitude of the "upper/right" geopoint
 * @param double $east
 *   The latitude of the "upper/right" geopoint
 * @param double $south
 *   The longitude of the "lower/left" geopoint
 * @param double $west
 *   The latitude of the "lower/left" geopoint
 *
 * @return array
 *   A one-dimensional array with the lon of the geopoint,
 *   the lat of the geopoint and the radius
 */
function whami_source_adapter_convert_bbox_geopoint_radius($north, $east, $south, $west) {
  $dist_two_lon = 40075.017 / 360;
  $dist_two_lat = 20003.9315 / 180;

  $dist_lon = 0;
  $lat = 0;
  if ($east >= $west) {
    $dist_lon = ($east - $west) * $dist_two_lon;
    $lon = 0.5 * ($east + $west);
  }
  else {
    $dist_lon = (360 + $east - $west) * $dist_two_lon;
    $lon = 180 + (0.5 * ($east + $west));
  }

  if ($lon > 180) {
    $lon = $lon - 360;
  }

  $dist_lat = ($north - $south) * $dist_two_lat;
  $lat = 0.5 * ($north + $south);

  $radius = 0.5 * sqrt(($dist_lon * $dist_lon) + ($dist_lat * $dist_lat));

  return array('lon' => $lon, 'lat' => $lat, 'radius' => $radius);
}

/**
 * Convert a "geographic circle" into a "bounding box"
 *
 * "geographic circle" a geographic point and a radius
 * "bounding box" denoted by 2 geopoints that nears that "geographic circle"
 *
 * @param double $lon
 *   The longitude of the geographic point
 * @param double $lat
 *   The latitude of the geographic point
 * @param int $radius
 *   The radius of the geographic circle
 *
 * @return array
 *   A one-dimensional array denoting two geograpic points which
 *   combined form a geographic bounding box
 */
function whami_source_adapter_convert_geopoint_radius_bbox($lon, $lat, $radius) {
  $earth_length = 40075.017;
  $earth_breadth = 20003.9315;

  $length = ($radius * 360 / $earth_length);
  $east = $lon + $length;
  $west = $lon - $length;

  $east = $east <= 180 ? $east : $east - 360;
  $west = $west >= -180 ? $west : $west + 360;

  $breadth = ($radius * 180 / $earth_breadth);
  $north = $lat + $breadth;
  $south = $lat - $breadth;

  $north = $north > 90 ? 90 : $north;
  $south = $south < -90 ? -90 : $south;

  return array(
    'north' => $north,
    'east' => $east,
    'south' => $south,
    'west' => $west,
  );
}

/**
 *
 * Takes the datasource nodes and generates the datasource list
 * @param $whami_datasources
 */
function whami_build_data_sources($whami_datasources = array(), $render = true, $add_to_js = false) {
//  if (empty($whami_datasources)) {
    $whami_datasources = whami_get_datasource_modules();
//  }

//  if (empty($whami_datasources)) {
    $whami_datasources = whami_get_datasource_nodes($whami_datasources);
//  }

  //Give modules a chance to modify the result
  foreach (module_implements('whami_source_list') as $module_name) {
    $function_name = $module_name . '_' . 'whami_source_list';
    $whami_datasources = $function_name($whami_datasources);
  }

  if ($add_to_js) {
    $whami_js_settings = array('data_sources' => $whami_datasources);
    drupal_add_js(array('whami' => $whami_js_settings), "setting");
  }

  return $whami_datasources;
}

/**
 * Create a map of the whami source nodes and their categories
 *
 * @return Map that associates each source with its categories
 */
function whami_get_datasource_nodes($datasources = array()) {

  // Get All the Data Sources
  $sources = array();
  $query = db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE n.type = 'whamidatasource'"));
  while ($result = db_fetch_array($query)) {
    $node = node_load($result);
    $sources[$node->field_programname[0]['value']] = $node;
  }

  // Get the nid for each category
  $query = db_query("SELECT nid, field_whamicategoryid_value FROM {content_type_whamicategories}");
  while ($result = db_fetch_array($query)) {
    $nids[$result['nid']] = $result['field_whamicategoryid_value'];
    $node = node_load($result['nid']);
    $categories[$result['field_whamicategoryid_value']] = $node;
  }

  if (!empty($sources)) {
    foreach($sources as $key => $value) {
      if(!is_array($datasources[$key])) $datasources[$key] = array();
      $datasources[$key]['source_id'] = $key;
      $datasources[$key]['source_name'] = $value->title;
      $datasources[$key]['own_whami_content'] = strcmp($value->field_whamiowncontent[0]['value'], 'yes') == 0 ? true : false;

      $datasources[$key]['source_image']['filepath'] = url($GLOBALS['base_url'] . '/'. $value->field_sourceimage[0]['filepath']);
      $datasources[$key]['source_logo']['filepath'] = url($GLOBALS['base_url'] . '/' . $value->field_sourcelogo[0]['filepath']);
      $datasources[$key]['map_display_icon'] = url($GLOBALS['base_url'] . '/' . $value->field_mapdisplayicon[0]['filepath']);

      $datasources[$key]['titleshrink'] = isset($value->field_titleshrink[0]['value']) ? $value->field_titleshrink[0]['value'] : NULL;
      $datasources[$key]['bodyshrink'] = isset($value->field_bodyshrink[0]['value']) ? $value->field_bodyshrink[0]['value'] : NULL;
      $datasources[$key]['linkshrink'] = isset($value->field_linkshrink[0]['value']) ? $value->field_linkshrink[0]['value'] : NULL;

      $datasources[$key]['maxRequestRows'] = isset($value->field_maxrequestrows[0]['value']) ? $value->field_maxrequestrows[0]['value'] : NULL;
      $datasources[$key]['maxRows'] = isset($value->field_maxrows[0]['value']) ? $value->field_maxrows[0]['value'] : NULL;

      $whami_categories = $value->field_whamicategories;
      if (!empty($whami_categories)) {
        $cats = array();

        foreach ($whami_categories as $category => $nid) {
          $cats[$nids[$nid['nid']]] = $categories[$nids[$nid['nid']]];
        }

        $datasources[$key]['whami_categories'] = $cats;
      }

      // Theming
      if ($render) {
        $datasources[$key]['source_image']['themed'] = theme_image($datasources[$key]['source_image']['filepath'], $datasources[$key]['source_name'], $datasources[$key]['source_name'], null, false);
      }

//      $sources[$key] = $datasource;
    }
  }

  return $datasources;
}

/**
 * return an array of Whami Source Adapter sources keyed by source ID
 */
function whami_get_datasource_modules() {
  module_load_include('inc', 'whami_source_adapter');
  $whami_datasources = array();

  $whami_source_modules = preg_grep('/whami_source_adapter_[a-z]+/i', module_list());
  if (!empty($whami_source_modules)) {
    foreach($whami_source_modules as $source_module) {
      $source_id = substr($source_module, strlen('whami_source_adapter_'));
      if (strlen($source_id)) {
        $whami_datasources[$source_id] = whami_get_datasource_module($source_id, $source_module);
      }
    }
  }

  return $whami_datasources;
}

function whami_get_datasource_module($source_id, $source_module) {
  global $base_url;

  $source_name = strtoupper($source_id[0]) . substr($source_id, 1);
  $module_path = drupal_get_path('module', $source_module);
  $wsa_module_path = drupal_get_path('module', 'whami_source_adapter');

  $source_image_path = $module_path . '/images/' . $source_id . '-image.jpg';
  if (!file_exists($source_image_path)) {
    $source_image_path = $wsa_module_path . '/images/default-image.jpg';
  }

  $source_logo_path = $module_path . '/images/' . $source_id . '-logo.png';
  if (!file_exists($source_logo_path)) {
    $source_logo_path = $wsa_module_path . '/images/default-logo.png';
  }

  $map_display_icon_path = $module_path . '/images/' . $source_id . '-marker.png';
  if (!file_exists($map_display_icon_path)) {
    $map_display_icon_path = $wsa_module_path . '/images/default-marker.png';
  }

  $source = array(
      'source_id' => $source_id,
      'source_name' => $source_name,
      'own_whami_content' => FALSE,
      'source_image' => array(
        'filepath' => url($base_url . '/' . $source_image_path),
      ),
      'source_logo' => array(
        'filepath' => url($base_url . '/' . $source_logo_path),
       ),
      'map_display_icon' => url($base_url . '/' . $map_display_icon_path),
      'titleshrink' => WHAMI_SOURCE_ADAPTER_TITLESHRINK,
      'bodyshrink' => WHAMI_SOURCE_ADAPTER_BODYSHRINK,
      'linkshrink' => WHAMI_SOURCE_ADAPTER_LINKSHRINK,
      'maxRows' => WHAMI_SOURCE_ADAPTER_MAX_ROWS,
      'maxRequestRows' => WHAMI_SOURCE_ADAPTER_MAX_REQUEST_ROWS,
  );

  //Give the module a chance to modify the result
  $function_name = $source_module . '_' . 'whami_source_info';
  if(function_exists($function_name)) {
    $source = $function_name($source);
  }

  return $source;
}

/**
 *
 * @param unknown_type $source_id
 * @param unknown_type $method
 * @param unknown_type $source_exists
 * @param unknown_type $method_exists
 */
function whami_source_adapter_set_error_message($source_id, $method, $source_exists, $method_exists, $critical = false) {
  module_load_include('inc', 'whami_source_adapter', 'whami_source_adapter.admin');

  if(!$critical && !_whami_source_adapter_debug()) return;
  if (isset($source_id)) {
    $args = array("@name" => $source_id, '@method' => $method);

    if(!$source_exists) {
      $msg = t("The required whami data source '@name' is not available. Please install the whami_source_adapter_@name module", $args);
      drupal_set_message($msg, 'error');
    }

    if (isset($method)) {
      $msg = t("The required method '@method' is not available. Check module whami_source_adapter_@name", $args);
      drupal_set_message($msg, 'error');
    }
  }
}

//function whami_get_sessionID() {
//  $sessionID = ip_address(). "_" . date('Y-m-d') . "_" . time();
//
//  return $sessionID;
//}

