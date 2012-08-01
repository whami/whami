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
          'feature',
      ),
  );

  $result = geonames_query('wikipediaboundingbox', $query, $parameters);

  $ret = array();

  if (is_object($result) && isset($result->results)) {
    $ret = whami_source_adapter_wikipedia_return($result->results, $lan);
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
          'feature',
      ),
  );

  $result = geonames_query('findnearbywikipedia', $query, $parameters);

  $ret = array();

  if (is_object($result) && isset($result->results)) {
    $ret = whami_source_adapter_wikipedia_return($result->results, $lan);
  }

  return $ret;
}

function whami_source_adapter_wikipedia_get_place_by_url($wiki_link, $langcode = '') {
  // http://en.wikipedia.org/w/api.php?action=query&prop=info|extracts&inprop=url|displaytitle&titles=Whitehorse%2C_Yukon&explaintext=1&exsentences=2&format=json

  $results = array();
  $langcode = strlen($langcode) ? $langcode : $GLOBALS['language']->language;

  $http_request = 'http://' . $langcode . '.wikipedia.org/w/api.php?';

  $http_query = array();
  $http_query['action'] = 'query';
  $http_query['prop'] = 'info|extracts';
  $http_query['inprop'] = 'url|displaytitle';

  // Get last url fragment
  $wiki_titles = substr($wiki_link, strrpos($wiki_link, "/") + 1);
  $wiki_titles = urldecode($wiki_titles);
  $wiki_titles = str_replace("_", " ", $wiki_titles);
  $http_query['titles'] = $wiki_titles;

  $http_query['explaintext'] = 1;
  $http_query['exsentences'] = 2;

  $http_query['format'] = 'json';

  // Make sure the HTTP request is valid and arguments combined with a '&'
  $http_request .= http_build_query($http_query, '', '&');

  // Commence the HTTP request towards Panoramio.
  $http_reply = drupal_http_request($http_request);

  if (!strcmp($http_reply->status_message, 'OK') && function_exists('json_decode')) {
    $data = json_decode($http_reply->data);

    if (is_object($data) && isset($data->query->pages) ) {
      foreach ($data->query->pages as $key => $value) {
        if (strcmp($value->fullurl, urldecode($wiki_link)) == 0
            || strcmp($value->fullurl, $wiki_link) == 0) {
          $results = whami_source_adapter_wikipedia_return(array((array) $value), $langcode);
        }
      }
    }
  }

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
  $langcode = strlen($langcode) ? $langcode : $GLOBALS['language']->language;

  if (isset($result)) {
    foreach ($result as $key => $value) {
      $results[] = array(
          'title' => $value['title'],
          'body' => strlen($value['summary']) ? $value['summary'] : $value['extract'],
          'type' => strlen($value['feature']) ? $value['feature'] : 'Article',
          'url' => strlen($value['wikipediaurl']) ? $value['wikipediaurl'] : $value['fullurl'],
          'full_url' => strlen($value['wikipediaurl']) ? $value['wikipediaurl'] : $value['fullurl'],
          'latitude' => $value['lat'],
          'longitude' => $value['lng'],
          'source_id' => 'wikipedia',
          'elevation' => $value['elevation'],
          'distance' => isset($value['distance']) ? $value['distance'] : NULL,
          'licence' => '<div class="whami_wiki_licence">' . t('All text is available under the terms of the GNU Free Documentation License.', array(), $langcode) . '</div>',
      );
    }
  }

  return $results;
}
