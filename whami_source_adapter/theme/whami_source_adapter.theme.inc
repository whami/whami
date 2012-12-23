<?php
function _whami_source_adapter_get_theme_hook_array() {
  return array(
    'whami_place_preview' => array(
      'arguments' => array('whami_place' => array(), 'langcode' => ''),
      'path' => drupal_get_path('module', 'whami_source_adapter') . '/theme',
      'file' => 'whami_source_adapter.theme.inc',
      'template' => 'whami_place_preview',
    ),
    'whami_place_list' => array(
      'arguments' => array('whami_place_list' => array(), 'whami_op' => 'teaser', ), //'source' => '', 'langcode' => ''
      'path' => drupal_get_path('module', 'whami_source_adapter') . '/theme',
      'file' => 'whami_source_adapter.theme.inc',
      'template' => 'whami_place_list',
    ),
    'whami_place_description' => array(
      'arguments' => array('whami_place' => array(), 'whami_short_desc' => false),
      'path' => drupal_get_path('module', 'whami_source_adapter') . '/theme',
      'file' => 'whami_source_adapter.theme.inc',
      'template' => 'whami_place_description',
    ),
    'whami_place_license' => array(
      'arguments' => array('whami_place' => array(), 'show_icon' => true),
      'path' => drupal_get_path('module', 'whami_source_adapter') . '/theme',
      'file' => 'whami_source_adapter.theme.inc',
      'template' => 'whami_place_license',
    ),
    'whami_place_bubble' => array(
      'arguments' => array('whami_place' => array()),
      'path' => drupal_get_path('module', 'whami_source_adapter') . '/theme',
      'file' => 'whami_source_adapter.theme.inc',
      'template' => 'whami_place_bubble',
    ),
  );
}

/**
 *
 * Theme place preview
 *
 * Variables to pass to the template:
 *  $action_link
 *  $whami_place_description
 */
function template_preprocess_whami_place_preview(&$variables) {
  $client_place = $variables['whami_place'];

  $link_to_place = "<a  ";
  $link_to_place .= "href=\"" .  $client_place['node_url'] . "\" ";
  $link_to_place .= "title=\"" . $client_place['url_hash'] . "\" ";
  $link_to_place .= "class=\"place-link\" ";
  $link_to_place .= ">";
  $link_to_place .= $client_place['title'];
  $link_to_place .= "</a>";

  $variables['action_link'] = $link_to_place;

  if (!$client_place['whami_place_description'])
    $client_place['whami_place_description'] = theme('whami_place_description', $client_place);

  $variables['whami_place_description'] = $client_place['whami_place_description'];
}

/**
 *
 * Theme place bubble
 *
 * Variables to pass to the template:
 *  $action_link
 *  $whami_place_description
 */
function template_preprocess_whami_place_bubble(&$variables) {
  $client_place = $variables['whami_place'];

  if (!$client_place['whami_place_preview'])
    $client_place['whami_place_preview'] = theme('whami_place_preview', $client_place);

   $variables['whami_place_preview'] = $client_place['whami_place_preview'];
}

function template_preprocess_whami_place_list(&$variables) {
  foreach ($variables['whami_place_list'] as $key => $client_place) {
    $list_items[] = array(
      'data' => array($client_place['teaser']), //'whami_op'
      'class' => 'whami-source-adapter-place-item',
      'id' => 'whami-source-adapter-place-item-' . $key,
//      'style' => 'list-style: none',
    );
  }

  $list_attributes = array(
    'class' => 'whami-source-adapter-place-list',
//    'style' => 'list-style: none',
  );

  $variables['whami_places'] = theme_table(null, $list_items, $list_attributes);
//  $variables['whami_places'] = theme_item_list($list_items, NULL, 'ul', $list_attributes);
}

/**
 *
 * Theme place description
 *
 * Variables to pass to the template:
 *  $whami_place_licence
 */
function template_preprocess_whami_place_description(&$variables) {
  $client_place = $variables['whami_place'];

  $variables['whami_place_descr'] = $client_place['teaser'] ? $client_place['teaser'] : $client_place['body'];

  if(!$client_place['whami_place_license'])
    $client_place['whami_place_license'] = theme('whami_place_license', $variables['whami_place'], !$variables['whami_short_desc']);

  $variables['whami_place_license'] = $client_place['whami_place_license'];
}

/**
 *
 * Theme place license
 *
 * Variables to pass to the template:
 *  $whami_place_icon
 *  $whami_ext_place_link
 */
function template_preprocess_whami_place_license(&$variables) {
  $client_place = $variables['whami_place'];

  if (!$client_place['own_whami_content']) {
    $variables['whami_ext_place_link'] =
      l(
        $client_place['source_name'] . ' >>',
        $client_place['full_url'],
        array('attributes' =>
                array('class' => 'actions place_full_url', 'target' => '_blank', 'title' => $client_place['full_url']),

        )
      );

    if ($variables['show_icon'])
      $variables['whami_place_icon'] =
        theme_image(
          $client_place['source_logo'],
          $client_place['source_name'],
          $client_place['source_name'],
          array('align' => "middle"),
          false
        );
  }
}