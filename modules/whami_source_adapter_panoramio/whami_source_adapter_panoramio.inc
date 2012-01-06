<?php

/**
 * @file
 * 
 * A collection of functions which use Panoramio's API
 */

/**
 *
 * Enter description here ...
 * @param unknown_type $result
 */
function whami_get_information_panoramio_return($result) {
  foreach ($result as $key => $value) {
    $img_attributes = array('width' => $result[$key]['width'], 'height' => $result[$key]['height']);
    $img = theme_image($result[$key]['photo_file_url'], $result[$key]['title'], $result[$key]['title'], $img_attributes, FALSE);

    $link = l($img, $result[$key]['full_url'], array('HTML' => TRUE, 'attributes' => array('target' => '_blank')));
    $author_link = l(htmlentities($result[$key]['owner_name']), $result[$key]['owner_url'], array('attributes' => array('target' => '_blank')));

    $body = $link;
    $body .= '<p>' . t('Author') . ': ' . $author_link . '</p>';
    $body .= '<p>' . t('Photos provided by Panoramio are under the copyright of their owners.') . '</p>';

    $result[$key]['body'] = $body;
  }

  return $result;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $bbox
 * @param unknown_type $lan
 * @param unknown_type $maxRequestRows
 * @param unknown_type $imgSize
 */
function whami_get_information_bounding_box_panoramio($bbox = NULL, $lan = "", $max_request_rows = 20, $img_size = 'thumbnail') {
  $results = array();

  $panoramio_http_request = 'http://www.panoramio.com/map/get_panoramas.php?';

  $panoramio_http_query = array();
  $panoramio_http_query['set'] = 'public';
  $panoramio_http_query['from'] = 0;
  $panoramio_http_query['mapfilter'] = 'true';
  $panoramio_http_query['size'] = $img_size;

  $panoramio_http_query['to'] = $max_request_rows;
  $panoramio_http_query['minx'] = $bbox['west'];
  $panoramio_http_query['miny'] = $bbox['south'];
  $panoramio_http_query['maxx'] = $bbox['east'];
  $panoramio_http_query['maxy'] = $bbox['north'];

  // make sure the HTTP request is valid and arguments combined with a '&'
  $panoramio_http_request .= http_build_query($panoramio_http_query, '', '&');

  // commence the HTTP request towards Google
  $panoramio_http_reply = drupal_http_request($panoramio_http_request);

  if (!strcmp($panoramio_http_reply->status_message, 'OK') && function_exists('json_decode')) {
    $data = json_decode($panoramio_http_reply->data);

    if (is_object($data) && isset($data->count) && $data->count > 0 && !empty($data->photos)) {
      foreach ($data->photos as $photo) {
        $row = array();
        $row['title'] = isset($photo->photo_title) ? $photo->photo_title : '';

        if (isset($photo->photo_url)) {
          $row['url'] = $photo->photo_url;
          $row['full_url'] = $photo->photo_url;
          $row['url_hash'] = hash("md5", $photo->photo_url);
        }
        else {
          continue;
        }

        $row['latitude'] = isset($photo->latitude) ? $photo->latitude : 0;
        $row['longitude'] = isset($photo->longitude) ? $photo->longitude : 0;

        // Additional fields, not covered by Data Staging API
        $row[$img_size . '_url'] = isset($photo->photo_file_url) ? $photo->photo_file_url : '';
        $row['photo_file_url'] = isset($photo->photo_file_url) ? $photo->photo_file_url : '';

        $row['width'] = isset($photo->width) ? $photo->width : '';
        $row['height'] = isset($photo->height) ? $photo->height : '';

        $row['owner_url'] = isset($photo->owner_url) ? $photo->owner_url : '';
        $row['owner_name'] = isset($photo->owner_name) ? $photo->owner_name : '';

        $results[$row['url_hash']] = $row;
      }

      return whami_get_information_panoramio_return($results);
    }
  }

  return $results;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $lat
 * @param unknown_type $lon
 * @param unknown_type $radius
 * @param unknown_type $lan
 * @param unknown_type $maxRequestRows
 * @param unknown_type $imgSize
 */
function whami_get_information_geopoint_radius_panoramio($lat = 0, $lon = 0, $radius = 5, $lan = "", $max_request_rows = 20, $img_size = 'thumbnail') {
  $results = array();

  $bbox = convert_geopoint_radius_bounding_box($lon, $lat, $radius);

  return whami_get_information_bounding_box_panoramio($bbox, $lan, $max_request_rows, $img_size);
}

/**
 *
 * Enter description here ...
 * @param unknown_type $result
 * @param unknown_type $result_preview
 * @param unknown_type $imgSize_preview
 */
function whami_panoramio_merge_preview_pics($result, $result_preview, $img_size_preview) {
  foreach ($result as $key => $value) {
    $result[$key][$img_size_preview . '_url'] = $result_preview[$key][$img_size_preview . '_url'];
    // hash('md5', $result_preview[$key]['full_url'])
  }
  return $result;
}