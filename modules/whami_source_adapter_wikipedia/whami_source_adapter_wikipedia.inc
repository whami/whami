<?php

/**
 * @file
 * A collection of functions to enable the usage of the GeoNames: Wikipedia service
 */

/**
 * Function that extracts information of the geonames service from a given Bounding Box
 * @param unknown_type $bbox
 * @param unknown_type $lan
 * @param unknown_type $categoriesID
 * @param unknown_type $sourceID
 * @param unknown_type $max_request_rows
 *
 * @return The results of the geonames webservice request
 */
function whami_get_information_bounding_box_wikipedia($bbox, $lan, $max_request_rows) {
  $query = array('north' => $bbox['north'], 'south' => $bbox['south'], 'east' => $bbox['east'], 'west' => $bbox['west'], 'lang' => $lan, 'maxRows' => $max_request_rows);
  $result = geonames_query('wikipediaboundingbox', $query, array('columns' => array('title', 'summary', 'wikipediaurl', 'lat', 'lng', 'elevation')));

  $ret = array();

  if (is_object($result) && isset($result->results)) {
    $ret = whami_source_adapter_wikipedia_return($result, $lan);
  }

  return $ret;
}

/**
 * Function that extracts information of the geonames service from a given geopoint and radius
 *
 * @param unknown_type $lat
 * @param unknown_type $lon
 * @param unknown_type $radius
 * @param unknown_type $lan
 * @param unknown_type $categoriesID
 * @param unknown_type $sourceID
 * @param unknown_type $max_request_rows
 *
 * @return The results of the geonames webservice request
 */
function whami_get_information_geopoint_radius_wikipedia($lat, $lon, $radius, $lan, $max_request_rows) {
  $query = array('lat' => $lat, 'lng' => $lon, 'radius' => $radius, 'lang' => $lan, 'maxRows' => $max_request_rows);

  $result = geonames_query('findnearbywikipedia', $query, array('columns' => array('title', 'summary', 'wikipediaurl', 'lat', 'lng', 'distance', 'elevation')));

  $results = whami_source_adapter_wikipedia_return($result, $lan);

  return $results;
}

/**
 * Function that adapts the result of the geonames websrvice to the standard Source Adapter format
 *
 * @param unknown_type $result
 * @param unknown_type $sourceID
 *
 * @return The parsed geonames webservice response
 */

function whami_source_adapter_wikipedia_return($result, $langcode) {
  $results = array();

  if (isset($result->results)) {
    foreach ($result->results as $key => $value) {
      $results[] = array(
      'categoryID' => '01',
      'title' => $value['title'],
      'body' => $value['summary'],
      'url' => $value['wikipediaurl'],
      'full_url' => $value['wikipediaurl'],
      'latitude' => $value['lat'],
      'longitude' => $value['lng'],
      'elevation' => $value['elevation'],
      'distance' => isset($value['distance']) ? $value['distance'] : NULL,
      'license' => '<div class="whami_wiki_license">' . t('All text is available under the terms of the GNU Free Documentation License.', array(), $langcode) . '</div>'
      );
    }
  }

  return $results;
}