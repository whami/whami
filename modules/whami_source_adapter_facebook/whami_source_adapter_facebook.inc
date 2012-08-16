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
function whami_source_adapter_facebook_geopoint_radius($lat = 0, $lon = 0, $radius = 5, $lan = "", $max_request_rows = 20, $mode = 'places') {
  $radius = $radius * WHAMI_FACEBOOK_FACTOR;

  if(strcmp($mode, 'places') == 0) {
    $fb_places = whami_get_facebook_places_popular($lat, $lon, $radius, $max_request_rows);
  }

  if(strcmp($mode, 'events') == 0) {
    $fb_places = whami_source_adapter_facebook_request($lat, $lon, $radius, $max_request_rows);
  }

  if(strcmp($mode, 'wall') == 0) {
    $fb_places = whami_source_adapter_facebook_walls($lat, $lon, $radius, $max_request_rows);
  }

  return whami_get_information_facebook_return($fb_places, $mode);
}

/**
 *
 * @param unknown_type $bbox
 * @param unknown_type $lan
 * @param unknown_type $max_request_rows
 * @param unknown_type $mode
 */
function whami_source_adapter_facebook_bounding_box($bbox, $lan = "", $max_request_rows = 20, $mode = 'places') {
  module_load_include('inc', 'whami_source_adapter', 'whami_source_adapter_toolkit');
  $geopoint = whami_source_adapter_convert_bbox_geopoint_radius($bbox['north'], $bbox['east'], $bbox['south'], $bbox['west']);
  return whami_source_adapter_facebook_geopoint_radius($geopoint['lat'], $geopoint['lon'], $geopoint['radius'], $lan, $max_request_rows, $mode);
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
function whami_get_facebook_places_popular($lat, $lon, $radius, $maxRequestRows, $token = false) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook_toolkit');

  $results = array();

  $param['fields'] = 'id,name,picture,link,location,likes,checkins,category,about,description';
  $param['type'] = 'place';
  $param['center'] = $lat . ',' . $lon;
  $param['distance'] = (int) $radius;
  $param['limit'] = $maxRequestRows;

  $fb_query_data = whami_execute_graph_api_request('search', $param);

  if (is_array($fb_query_data['data'])) { // && !empty($data->results) && is_array($data->results)) {
    $data = $fb_query_data['data'];
    foreach ($data as $key => $value) {
      $row = array();

      $row['id'] = $data[$key]['id'];
      $row['title'] = $data[$key]['name'];
      $row['picture'] = $data[$key]['picture'];

      $row['likes'] = $data[$key]['likes'];
      $row['checkins'] = $data[$key]['checkins'];
      $row['points'] = (int) $data[$key]['likes'] + (int) $data[$key]['checkins'];

      $row['category'] = $data[$key]['category'];
      $row['about'] = $data[$key]['about'];
      $row['description'] = $data[$key]['description'];

      $row['latitude'] = $data[$key]['location']['latitude'];
      $row['longitude'] = $data[$key]['location']['longitude'];

      $row['url'] = whami_source_facebook_get_url_prefix() . $data[$key]['id'];

      $results[$row['points']] = $row;
    }
  }

  return $results;
}

/**
 * @param $page_id A Facebook page ID
 */
function whami_get_facebook_get_place_by_url($page_id) {
  module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook_toolkit');

  $results = array();

  $param['fields'] = 'id,name,picture,link,location,likes,checkins,category,about,description';
  $fb_query_data = whami_execute_graph_api_request($page_id, $param);

  if (isset($fb_query_data['data'])) {
    $data = $fb_query_data['data'];
    $data['points'] = (int) $data['likes'] + (int) $data['checkins'];
    $data['latitude'] = $data['location']['latitude'];
    $data['longitude'] = $data['location']['longitude'];
    $data['url'] = whami_source_facebook_get_url_prefix() . $data['id'];
    $results[] = $data;
  }

  return $results;
}

/**
 *
 * @param unknown_type $fb_places
 * @param unknown_type $mode
 */
function whami_get_information_facebook_return($fb_places, $mode) {
  $result = array();

  if (!empty($fb_places) && is_array($fb_places)) {
    foreach($fb_places as $fb_page_id => $fb_place) {
      $result[] = array(
          'title' => $fb_place['title'],
          'body' => $fb_place['about'] ?  $fb_place['about'] : $fb_place['description'],
          'url' => $fb_place['url'],
          'full_url' => $fb_place['url'],
          'latitude' => $fb_place['latitude'],
          'longitude' => $fb_place['longitude'],
          'source_id' => 'facebook',
          'license' => '',
      );
    }
  }

  return $result;
}
