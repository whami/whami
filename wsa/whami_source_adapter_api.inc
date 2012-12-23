<?php
/**
 * @file
 * whami Source Adapter api
 */

/**
 * Get data from a source adapter in a certain "bounding box"
 *
 * @param array $bbox
 *   bounding box
 * @param string $lan
 *   language
 * @param array $categories_id
 *   categories
 * @param string $source_id
 *   source id
 * @param int $max_rows
 *   maximum rows to return
 * @param int $titleshrink
 *   titleshrink
 * @param int $bodyshrink
 *   titleshrink
 * @param int $linkshrink
 *   titleshrink
 * @param int $max_request_rows
 *   maximum rows to request
 * @param string $session_id
 *   session id
 *
 * @return array
 *   Parsed response from the specific source adapter
 */
function whami_source_adapter_bounding_box($bbox, $source_id, $langcode, $max_rows = 5, $max_request_rows = 5, $session_id = '') {
  $params = array($bbox, $langcode, $max_request_rows);
  return whami_source_adapter_call_method('bounding_box', $params, $source_id, $langcode, $max_rows, true, true, $session_id);
}

/**
 * Get data from a source adapter at a certain Geopoint and Radius
 *
 * @param $lat
 * @param $lon
 * @param int $radius
 * @param string $source_id
 * @param char2 $langcode
 * @param int $max_rows
 * @param int $max_request_rows
 * @param string $session_id
 */
function whami_source_adapter_geopoint_radius($lat, $lon, $radius, $source_id, $langcode = '',  $max_rows = 5, $max_request_rows = 5, $session_id = '') {
  $params = array($lat, $lon, $radius, $langcode, $max_request_rows);
  return whami_source_adapter_call_method('geopoint_radius', $params, $source_id, $langcode, $max_rows, true, true, $session_id);
}

/**
 * Get data from certain categories of source adapters at a certain Geopoint and Radius
 *
 * @param double $lat
 *   latitude
 * @param double $lon
 *   longitude
 * @param int $radius
 *   radius
 * @param string $lan
 *   language
 * @param array $categories_id
 *   categories
 * @param string $source_id
 *   source id
 * @param int $max_rows
 *   maximum rows to return
 * @param int $titleshrink
 *   titleshrink
 * @param int $bodyshrink
 *   titleshrink
 * @param int $linkshrink
 *   titleshrink
 * @param int $max_request_rows
 *   maximum rows to request
 * @param string $session_id
 *   session id
 *
 * @return array
 *   Parsed response from the specific source adapter
 */
function whami_source_adapter_geopoint_radius_categories($lat, $lon, $radius, $langcode, $categories_id, $source_id, $max_rows, $titleshrink, $bodyshrink, $linkshrink, $max_request_rows, $session_id) {
  if (module_exists('whami_source_adapter_' . $source_id) && function_exists('whami_source_adapter_' . $source_id . '_geopoint_radius')) {
    $function = 'whami_source_adapter_' . $source_id . '_geopoint_radius';
    $result = $function($lat, $lon, $radius, $lan, $max_request_rows);
    return whami_source_adapter_parse_response($result, $max_rows, $titleshrink, $bodyshrink, $linkshrink, $categories_id, $source_id, $session_id);
  }
  else {
    drupal_set_message(t("The required Whami data source is not available. Please install the whami_source_adapter_@name module", array("@name" => $source_id)), 'error');
  }
}

/**
 * Get place from certain source adapter by url
 *
 * @param $url
 * @param $source_id
 * @param $langcode
 */
function whami_source_adapter_get_place_by_url($url, $source_id, $langcode = '', $session_id = '') {
  $params = array($url, $langcode);
  return whami_source_adapter_call_method('get_place_by_url', $params, $source_id, $langcode, 1, true, true, $session_id);
}

/**
 * Call a generic function on a source adapter
 */
function whami_source_adapter_call_method($method, $params, $source_id, $lang_code = '', $max_rows = 10, $render = true, $parse = true, $session_id = '') {
  $results = array();
  $sources = is_array($source_id) ?  $source_id : array($source_id);
  $params = is_array($params) ?  $params : array($params);

  //http://www.lullabot.com/articles/a-beginners-guide-to-caching-data
  static $method_results = array();
  $param_hash = implode(':', $params) . implode(':', $sources) . $GLOBALS['language']->language;
  $param_hash = hash("md5", $param_hash);
  if($method_results[$param_hash])
    return $method_results[$param_hash];

  foreach($sources as $source_id) {
    $source_id = drupal_strtolower($source_id);
    $module_name = 'whami_source_adapter_' . $source_id;
    $function_name = $module_name . '_' . $method;
    module_load_include('inc', $module_name);

//    $function = 'whami_source_adapter_' . $source_id . '_' . $method;

    $source_exists = module_exists($module_name);
    $method_exists = function_exists($function_name);

    if ($source_exists && $method_exists) {
      $result = call_user_func_array($function_name, $params);
      if(is_array($result) && count($result)) $results = array_merge($results, $result);
    }
    else {
      whami_source_adapter_set_error_message($source_id, $method, $source_exists, $method_exists);
    }
  }

//if($parse) {
    if(is_array($result) && count($result))
      $method_results[$param_hash] = whami_source_adapter_parse_response($results, '', $max_rows, $render, $session_id);
//}

  return $method_results[$param_hash];

}

