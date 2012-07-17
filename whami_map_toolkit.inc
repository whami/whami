<?php
function whami_set_map_settings($mapid, $mapType, $cluster = false) {

    $whami_js_settings = array();

    $whami_js_settings['whami_mapid'] = $mapid;
    $whami_js_settings['whami_mapType'] = $mapType;
    $whami_js_settings['whami_fb_path'] = url("");

//    if($cluster) {
//      $whami_js_settings['whami_map_cluster'] = $mapid;
//      drupal_add_js(drupal_get_path('module', 'whami_map') . '/js/' . 'whami_map_cluster.js', 'module');
//      drupal_add_js(drupal_get_path('module', 'whami_map') . '/js/' . 'markerclusterer_packed.js', 'module');
//    }

    drupal_add_js(array('whami' => $whami_js_settings), "setting");
    drupal_add_js(drupal_get_path('module', 'whami_map_toolkit') . '/js/' . 'whami_map_toolkit.js', 'module');

//    if(function_exists('whami_checkin_add_js')) : whami_checkin_add_js(); endif;
//    if(function_exists('whami_places_add_js')) : whami_places_add_js(); endif;
//    if(function_exists('whami_facebook_add_js')) : whami_facebook_add_js(); endif;
}

function whami_create_place_from_node($node, $field_name, $mapid) {
  $client_place = array();

  $client_place['source_id'] = 'node';
  //$client_place['source_name'] = sitename;
  //$client_place['source_image'] = source_image;

  $client_place['title'] = $node->title;
  $client_place['body'] = $node->body;
  $client_place['full_url'] = url('node/'. $node->nid);
  $client_place['url_hash'] = hash('md5', $client_place['full_url']);

  $client_place['nid'] = $node->nid;
//  $client_place['language'] = Drupal.settings.whami.lang,
//  own_whami_content: "yes",

  //http://drupal.org/node/699086#comment-3999780
  $cck_geo_field = $node->{$field_name}[0];
  $point = geo_value($cck_geo_field['wkb'], 'array', 'wkb');

  $client_place['latitude'] = substr((string)$point['lat'], 0, 11);
  $client_place['longitude'] = substr((string) $point['lon'], 0, 11);

//  category_name: "Points of Interest",
//  category_icon: {"filepath":"sites/default/files/marker6_0.png"},

  $client_place['mapid'] = $mapid;
//  map_display_icon: whami_location.map_display_icon,

  $infoBubble = '<div class="whami-infoBubble-body">';
  $infoBubble .= theme('node', $node, true); //node_view($node, 'teaser');
  $infoBubble .= '</div>';
  $client_place['infoBubble'] = $infoBubble;
//$client_place['infoBubble'] = whami_create_bubble($client_place, 'initial');
//$client_place['infoBubble'] = theme('whami_infoBubble', $client_place);

  return $client_place;
}
