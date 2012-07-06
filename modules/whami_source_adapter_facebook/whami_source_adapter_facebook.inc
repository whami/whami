<?php

module_load_include('inc', 'whami_source_adapter_facebook', 'whami_source_adapter_facebook_toolkit');
define('whami_facebook_factor', 1000);

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

  $radius = $radius * whami_facebook_factor;

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

function whami_get_facebook_places_popular($lat, $lon, $radius, $maxRequestRows, $token = false) {

  $results = array();

  $param['fields'] = 'id,name,picture,link,location,likes,checkins,category,about,description';
  $param['type'] = 'place';
  $param['center'] = $lat . ',' . $lon;
  $param['distance'] = $radius;
  $param['limit'] = $maxRequestRows;

  $fb_query_data = whami_execute_graph_api_request('search', $param);

      if ( is_array($fb_query_data['data']) ) {// && isset($data->results) && is_array($data->results) && count($data->results) > 0 ) {
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

function whami_get_information_facebook_return($fb_places, $mode) {

  $result = array();

  if ( is_array($fb_places) && !empty($fb_places)) {

//    if(strcmp($mode, 'wall') == 0) {
//
//    }

    foreach($fb_places as $fb_page_id => $fb_place) {

        $result[] = array(

            'title' => $fb_place['title'],

//            'body' => 'About: ' . $fb_place['about'] . ' DESC: ' . $fb_place['description'],
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
