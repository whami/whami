<?php

/**
 * Convert a bounding box into a point
 *
 * This function converts a bounding box of geographic data into a geographic
 * point and a radius that nears that bounding box
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
 * This function converts a "geographic circle" into
 * bounding box denoted by 2 geopoints
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
