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
 * Create a map of the whami source nodes and their categories
 *
 * @return Map that associates each source with its categories
 */
function whami_get_datasource_nodes() {
  //Get All the Data Sources
  $query = db_query(db_rewrite_sql("SELECT n.nid FROM {node} n WHERE n.type = 'whamidatasource'"));
  while ($result =db_fetch_array($query)){
    $node = node_load($result);
    $sources[$node->field_programname[0]['value']] = $node;
  }
  //Get the nid for each category
  $query = db_query("SELECT nid, field_whamicategoryid_value FROM {content_type_whamicategories}");
  while ($result = db_fetch_array($query)){
    $nids[$result['nid']] = $result['field_whamicategoryid_value'];
    $node = node_load($result['nid']);
    $categories[$result['field_whamicategoryid_value']] = $node;
  }

  foreach ($sources as $key => $value){
    $cats = array();
    foreach ($sources[$key]->field_whamicategories as $category => $nid){
      $cats[$nids[$nid['nid']]] = $categories[$nids[$nid['nid']]];
    }
    $sources[$key]->whami_categories = $cats;
  }

  return $sources;
}

/**
 *
 * Takes the datasource nodes and generates the datasource list
 * @param $datasource_map
 */
function whami_build_data_sources($datasource_map = array(), $render = true, $add_to_js = false) {
  if (!count($datasource_map)) $datasource_map = whami_get_datasource_nodes();

  foreach($datasource_map as $key => $value){
    $datasource = array();
    $datasource['source_id'] = $key;
    $datasource['source_name'] = $datasource_map[$key]->title;
    $datasource['own_whami_content'] = strcmp($value->field_whamiowncontent[0]['value'], 'yes') == 0 ? true : false;

    $datasource['source_image']['filepath'] = url($GLOBALS['base_url'] . '/'. $datasource_map[$key]->field_sourceimage[0]['filepath']);
//    $datasource['source_image']['filepath'] = url($GLOBALS['base_url'] . base_path() . $datasource_map[$key]->field_sourceimage[0]['filepath']);
    $datasource['source_logo']['filepath'] = url($GLOBALS['base_url'] . '/' . $datasource_map[$key]->field_sourcelogo[0]['filepath']);
    $datasource['map_display_icon'] = url($GLOBALS['base_url'] . '/' . $value->field_mapdisplayicon[0]['filepath']);

    $datasource['titleshrink'] = isset($value->field_titleshrink[0]['value']) ? $value->field_titleshrink[0]['value'] : NULL;
    $datasource['bodyshrink'] = isset($value->field_bodyshrink[0]['value']) ? $value->field_bodyshrink[0]['value'] : NULL;
    $datasource['linkshrink'] = isset($value->field_linkshrink[0]['value']) ? $value->field_linkshrink[0]['value'] : NULL;

    $datasource['maxRequestRows'] = isset($value->field_maxrequestrows[0]['value']) ? $value->field_maxrequestrows[0]['value'] : NULL;
    $datasource['maxRows'] = $value->field_maxrows[0]['value'];

    //Theming
    if ($render) {
      $datasource['source_image']['themed'] = theme_image($datasource['source_image']['filepath'], $datasource['source_name'], $datasource['source_name'], null, false);
    }

    $whami_datasources[$key] = $datasource;
  }

    if ($add_to_js) {
      $whami_js_settings = array('data_sources' => $whami_datasources);
      drupal_add_js(array('whami' => $whami_js_settings), "setting");
    }

  return $whami_datasources;
}

///**
// * Get the configured DataSources in format that can be passed to the client.
// *
// * @param $add_to_js boolean optional If you want to add the datasource to the Drupal.settings object.
// *      Default false, if you set to true, the following parameter whami_data_source_name must be provided.
// * @param $whami_data_source_name optional The name of the datasource for whami internal content
// */
//function whami_get_datasources_in_client_format($add_to_js = false, $whami_data_source_name = '', $datasource_map = 0) {
//  $whami_datasources = array();
//
//  if (!$datasource_map) $datasource_map = whami_get_datasource_category_mapping();
//  foreach($datasource_map as $key => $value) {
//    $datasource = array();
//    $datasource['source_name'] = $datasource_map[$key]->title;
//    $datasource['source_image']['filepath'] = url($GLOBALS['base_url'] . base_path() . $datasource_map[$key]->field_sourceimage[0]['filepath']);
//    $datasource['own_whami_content'] = $value->field_whamiowncontent[0]['value'];
//    $datasource['map_display_icon'] = url($GLOBALS['base_url'] . base_path() . $value->field_mapdisplayicon[0]['filepath']);
//    $whami_datasources[$datasource_map[$key]->title] = $datasource;
//  }
//
//  if ($add_to_js) {
//    $whami_js_settings = array('whami_source_name' => $whami_data_source_name, 'data_sources' => $whami_datasources);
//    drupal_add_js(array('whami' => $whami_js_settings), "setting");
//  }
//
//  return $whami_datasources;
//}

function whami_source_adapter_set_error_message($source_id, $method, $source_exists, $method_exists) {
       if(!$source_exists)
        drupal_set_message(t("The required whami data source '@name' is not available. Please install the whami_source_adapter_@name module", array("@name" => $source_id)), 'error');
//      else
        drupal_set_message(t("The required method '@method' is not available. Check module whami_source_adapter_@name", array("@name" => $source_id, '@method' => $method)), 'error');
}