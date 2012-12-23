<?php
function _whami_source_adapter_facebook_get_theme_hook_array() {
  return array(
    'whami_facebook_place_teaser' => array(
      'arguments' => array('whami_place' => array(), 'langcode' => ''),
      'path' => drupal_get_path('module', 'whami_source_adapter_facebook') . '/theme',
      'file' => 'whami_source_adapter_facebook.theme.inc',
      'template' => 'whami_facebook_teaser',
    ),
    'whami_facebook_event_teaser' => array(
      'arguments' => array('whami_event' => array()),
      'path' => drupal_get_path('module', 'whami_source_adapter_facebook') . '/theme',
      'file' => 'whami_source_adapter_facebook.theme.inc',
      'template' => 'whami_facebook_teaser',
    ),
  );
}

function template_preprocess_whami_facebook_place_teaser($variables, $hook) {
  _whami_facebook_theme_object_teaser('place', $place['title'], $fb_body->body, $fb_body->picture, $place['title'], $variables);
}

function template_preprocess_whami_facebook_event_teaser($variables, $hook) {
  $fb_post = $variables['whami_event'];

  if ($fb_post->event_timestamp) {
      $event_descr = format_date($fb_post->event_timestamp);
  }
  if ($fb_post->event_location) {
      $event_descr .= " @ " . $fb_post->event_location;
  }

  if(!$event_descr) $event_descr = $fb_post->message;

  if (strcmp($fb_post->type, 'whami_event_link') == 0) {
    $fb_event_link_options = array('external' => TRUE, 'attributes' => array('target' => '_blank', 'title' => $fb_post->message));
    $fb_event_link = l($fb_post->link_descr, $fb_post->link_url, $fb_event_link_options);
  }

  _whami_facebook_theme_object_teaser('post', $fb_event_link, $event_descr, $fb_post->picture, $fb_post->link_descr, $variables);
}

function _whami_facebook_theme_object_teaser($object_type, $title, $message, $picture = '', $pic_id = '', &$variables) { //$preset
  $variables['object_type'] = $object_type;
  $variables['whami_title'] = $title;
  $variables['whami_message'] = $message;

  if (isset($picture) && strlen($picture)) {
    //$picture = imagecache_external_generate_path($picture, $preset
    //$picture = whami_remove_language_prefix($picture);)
    $variables['whami_icon'] = theme_image($picture, $pic_id, $pic_id, array('height' => 50, 'width' => 50), false);
  }

//  whami_source_adapter_facebook_add_styles();
}
