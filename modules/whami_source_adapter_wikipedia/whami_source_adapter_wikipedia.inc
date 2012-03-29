<?php
/**
 * @file
 * A collection of functions to enable the usage of
 * the GeoNames: Wikipedia service
 */

/**
 * Function that extracts information of the geonames service
 * from a given Bounding Box
 *
 * @param array $bbox
 *   Bounding box
 * @param string $lan
 *   Language
 * @param int $max_request_rows
 *   Maximum rows to request
 *
 * @return array
 *   The results of the geonames webservice request
 */
function whami_source_adapter_wikipedia_bounding_box($bbox, $lan, $max_request_rows) {
  $query = array(
    'north' => $bbox['north'],
    'south' => $bbox['south'],
    'east' => $bbox['east'],
    'west' => $bbox['west'],
    'lang' => $lan,
    'maxRows' => $max_request_rows,
  );

  $parameters = array(
    'columns' => array(
      'title',
      'summary',
      'wikipediaurl',
      'lat',
      'lng',
      'elevation',
    ),
  );

  $result = geonames_query('wikipediaboundingbox', $query, $parameters);

  $ret = array();

  if (is_object($result) && isset($result->results)) {
    $ret = whami_source_adapter_wikipedia_return($result, $lan);
  }

  return $ret;
}

/**
 * Function that extracts information of the geonames service
 * from a given geopoint and radius
 *
 * @param double $lat
 *   Latitude
 * @param double $lon
 *   Longitude
 * @param int $radius
 *   Radius
 * @param string $lan
 *   Language
 * @param int $max_request_rows
 *   Maximum rows to request
 *
 * @return array
 *   The results of the geonames webservice request
 */
function whami_source_adapter_wikipedia_geopoint_radius($lat, $lon, $radius, $lan, $max_request_rows) {
  $query = array(
    'lat' => $lat,
    'lng' => $lon,
    'radius' => $radius,
    'lang' => $lan,
    'maxRows' => $max_request_rows,
  );

  $parameters = array(
    'columns' => array(
      'title',
      'summary',
      'wikipediaurl',
      'lat',
      'lng',
      'distance',
      'elevation',
    ),
  );

  $result = geonames_query('findnearbywikipedia', $query, $parameters);

  $results = whami_source_adapter_wikipedia_return($result, $lan);

  return $results;
}

/**
 * Function that adapts the result of the geonames websrvice
 * to the standard Source Adapter format
 *
 * @param array $result
 *   result
 * @param string $langcode
 *   Languagecode
 *
 * @return array
 *   The parsed geonames webservice response
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
        'license' => '<div class="whami_wiki_license">' . t('All text is available under the terms of the GNU Free Documentation License.', array(), $langcode) . '</div>',
      );
    }
  }

  return $results;
}
