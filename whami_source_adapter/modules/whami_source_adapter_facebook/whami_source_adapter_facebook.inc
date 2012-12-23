<?php
define('WHAMI_FACEBOOK_FACTOR', 1000);

/**
 * Get Facebook places within a bounding box
 *
 * @param unknown_type $lat
 * @param unknown_type $lon
 * @param unknown_type $radius
 * @param unknown_type $lan
 * @param unknown_type $max_request_rows
 * @param unknown_type $mode
 */
function whami_source_adapter_facebook_geopoint_radius($lat = 0, $lon = 0, $radius = 5, $lan = "", $max_request_rows = 20) { //$mode = 'places'
  $radius = $radius * WHAMI_FACEBOOK_FACTOR;

  if(function_exists('_whami_source_adapter_facebook_search_strategy')
      && $function_name = _whami_source_adapter_facebook_search_strategy())
    return $function_name($lat, $lon, $radius, $max_request_rows);
  else
    return whami_get_facebook_places_popular($lat, $lon, $radius, $max_request_rows);
}

/**
 *
 * @param unknown_type $bbox
 * @param unknown_type $lan
 * @param unknown_type $max_request_rows
 * @param unknown_type $mode
 */
function whami_source_adapter_facebook_bounding_box($bbox, $lan = "", $max_request_rows = 20) { //$mode = 'places'
  module_load_include('inc', 'whami_source_adapter', 'whami_source_adapter_toolkit');
  $geopoint = whami_source_adapter_convert_bbox_geopoint_radius($bbox['north'], $bbox['east'], $bbox['south'], $bbox['west']);
    // FIXME kbfix: $geopoint['radius'] seems to be too low most of the time
  if($geopoint['radius'] > 0 && $geopoint['radius'] < 1) $geopoint['radius'] = 1;
  return whami_source_adapter_facebook_geopoint_radius($geopoint['lat'], $geopoint['lon'], (int) $geopoint['radius'], $lan, $max_request_rows);
}

/**
 * @param $fb_url A URL to a Facebook page
 */
function whami_source_adapter_facebook_get_place_by_url($fb_url) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook_toolkit');

  $page_id = whami_facebook_get_id_from_url($fb_url);
  $results = array();

  $param['fields'] = 'id,name,picture,link,location,likes,checkins,category,about,description';
  $fb_query_data = whami_execute_graph_api_request($page_id, $param);

  if ($fb_query_data) {
    return whami_facebook_return_places_popular(array($fb_query_data));
  }

  return $results;
}

/**
 *
 * @param unknown_type $lat
 * @param unknown_type $lon
 * @param unknown_type $radius
 * @param unknown_type $maxRequestRows
 * @param unknown_type $token
 * @return multitype:multitype:NULL number string
 */
function whami_get_facebook_places_popular($lat, $lon, $radius, $maxRequestRows = 10, $token = false) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook_toolkit');

  $results = array();

  $param['fields'] = 'id,name,picture,link,location,likes,checkins,category,about,description';
  $param['type'] = 'place';
  $param['center'] = $lat . ',' . $lon;
  $param['distance'] = (int) $radius < 5000 ? (int) $radius : 5000;
  $param['limit'] = $maxRequestRows;

  $fb_query_result = whami_execute_graph_api_request('search', $param);

  if (is_array($fb_query_result['data']) && !empty($fb_query_result['data']) ) {
    return whami_facebook_return_places_popular($fb_query_result['data']);
  }

  return $results;
}

function whami_facebook_return_places_popular($data) {
  foreach ($data as $key => $value) {
    $row = array();

//    $row['id'] = $data[$key]['id'];
    $row['title'] = $data[$key]['name'];
//    $row['picture'] = $data[$key]['picture'];

    $row['category'] = $data[$key]['category'];

    if(function_exists('whami_facebook_set_place_data')) {
      whami_facebook_set_place_data($data[$key], $row);
    }

    if (!$data[$key]['body'] && isset($data[$key]['about']) && strlen($data[$key]['about'])) {
      $data[$key]['body'] = $data[$key]['about'];
    }
    else if(!$data[$key]['body'] && isset($data[$key]['description']) && strlen($data[$key]['description'])) {
      $data[$key]['body'] = $data[$key]['description'];
    }
    $row['body'] = $data[$key]['body'];

    $row['latitude'] = $data[$key]['location']['latitude'];
    $row['longitude'] = $data[$key]['location']['longitude'];

    $row['url'] = whami_facebook_get_place_url($data[$key]['id']);
    $row['points'] ? $results[$row['points']] = $row : $results[] = $row;
  }

  if($row['data']) krsort($results, SORT_NUMERIC);

  return whami_get_information_facebook_return($results);
}

function whami_get_information_facebook_return($fb_places = array()) {
  $result = array();
  if (is_array($fb_places) && !empty($fb_places)) {

    foreach($fb_places as $fb_page_id => $fb_place) {
      $result[] = array(
        'title' => $fb_place['title'],
        'body' => $fb_place['body'],
        'type' => strlen($fb_place['category']) ? $fb_place['category'] : 'Page',
        'url' => $fb_place['url'],
        'full_url' => $fb_place['url'],
        'feed_url' => whami_facebook_get_feed_url(whami_facebook_get_id_from_url($fb_place['url'])),
        'latitude' => $fb_place['latitude'],
        'longitude' => $fb_place['longitude'],
        'source_id' => 'facebook',
        'license' => '',
        'data' => $fb_place['data'],
        'elevation' => '',
        'distance' => isset($fb_place['distance']) ? $fb_place['distance'] : NULL,
      );
    }

  }
  return $result;
}
