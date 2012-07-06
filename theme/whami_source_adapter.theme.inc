<?php

drupal_add_css(drupal_get_path('module', 'whami_source_adapter') . '/theme/' . 'whami_source_adapter-style.css', 'module');

function template_preprocess_whami_place_teaser(&$variables) {
  if(is_object($variables['langcode'])) {
    $langcode = $variables['langcode']->language;
  }
  elseif (!strlen($variables['langcode'])) {
      $variables['langcode'] = $GLOBALS['language']->language;
  }

  $client_place = $variables['whami_place'];

  if($variables['whami_show_type'])
    $variables['whami_place_type'] = theme('whami_place_type', $client_place);

    $show_icon = !$variables['whami_show_type'] && !$client_place['own_whami_content'] ? true : false;

  $variables['whami_place_licence'] = theme('whami_place_licence', $client_place, $client_place['node_url'], $client_place['url'], $show_icon);

//  $variables['whami_place_icon'] = theme_image($client_place['map_display_icon'], $client_place['source_name'], $client_place['source_name'], array(), false);
  array_rand($variables);
}

function template_preprocess_whami_place_list(&$variables) {
//  if(is_object($variables['langcode'])) {
//    $langcode = $variables['langcode']->language;
//  }
//  elseif (!strlen($variables['langcode'])) {
//      $variables['langcode'] = $GLOBALS['language']->language;
//  }

  foreach ($variables['whami_place_list'] as $key => $client_place) {
//    $whami_places[] = $client_place['teaser'];

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

//  $variables['whami_places'] = theme_item_list($list_items, NULL, 'ul', $list_attributes);

  $variables['whami_places'] = theme_table(null, $list_items, $list_attributes);

}

function template_preprocess_whami_place_type(&$variables) {

  $client_place = $variables['whami_place'];

  if ($client_place['own_whami_content']) {
    $whami_node = node_load($client_place['nid']);

    if($variables['whami_short_desc']) {
      $variables['whami_place_type'] = t('Submitted on @datetime',
        array('@datetime' => format_date($whami_node->created)));
    }
    else {
     $variables['whami_place_type'] = theme('node_submitted', $whami_node);
    }
  }
  else {
    $variables['whami_place_type'] = $client_place['type'];
    $variables['whami_place_icon'] = theme_image($client_place['map_display_icon'], $client_place['source_name'], $client_place['source_name'], array(), false);
  }

}

function template_preprocess_whami_place_licence(&$variables) {

  $client_place = $variables['whami_place'];

    if ($client_place['own_whami_content']) {
      if ($variables['whami_short_desc']) return;
      $variables['whami_ext_place_link'] =
      l(
        'Read more >',
        $client_place['full_url'],
        array('attributes' =>
                array('class' => 'actions place_full_url', 'target' => '_top', 'title' => $client_place['full_url']),

        )
      );
    }
    else {
      $variables['whami_ext_place_link'] =
      l(
        'Read more on ' . $client_place['source_name'] . ' >',
        $client_place['full_url'],
        array('attributes' =>
                array('class' => 'actions place_full_url', 'target' => '_blank', 'title' => $client_place['full_url']),

        )
      );
    }

    if ($variables['show_icon'])
      $variables['whami_place_icon'] = theme_image($client_place['source_logo'], $client_place['source_name'], $client_place['source_name'], array('align' => "middle"), false);

}