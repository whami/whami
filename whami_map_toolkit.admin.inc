<?php
function _whami_map_toolkit_admin_settings_form() {

  $module_name = 'WHAMI_MAP';
  $source_id = 'TOOLKIT';

  $form = array();

  $form['content_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enabled content types'),
    '#description' => t('The content types for which this module will become active'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $content_types = _whami_map_toolkit_get_options_content_types();
  foreach($content_types as $type => $name) {
    $form['content_types'][$module_name . '_' . $source_id . '_CONTENT_TYPE_' . $type] = array(
      '#type' => 'checkbox',
      '#title' => t('%type', array('%type' => $name)),
      '#default_value' => _whami_map_toolkit_conf_variable_value('CONTENT_TYPE' . '_' . $type, array(), $source_id),
    );
  }

  $form[$module_name . '_' . $source_id . '_' . 'FIELD_GEO'] = array(
    '#type' => 'select',
    '#title' => t('Field: Geospatial information'),
    '#description' => t('The geo field from which geo spatial information will be used'),
    '#default_value' => _whami_map_toolkit_conf_variable_value('FIELD_GEO', ''),
    '#options' => _whami_map_toolkit_get_options_fields('geo'),
    '#multiple' => FALSE,
    '#required' => TRUE,
  );

  $form[$module_name . '_' . $source_id . '_' . 'SEARCH_RADIUS'] = array(
    '#type' => 'textfield',
    '#title' => t('Search radius'),
    '#description' => t('The radius in kilometers that places will be searched in'),
    '#default_value' => (int) _whami_map_toolkit_conf_variable_value('SEARCH_RADIUS', 5),
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
  );

  $form[$module_name . '_' . $source_id . '_' . 'MAX_RESULTS'] = array(
    '#type' => 'textfield',
    '#title' => t('Search limit'),
    '#description' => t('The max. amount of total places to return'),
    '#default_value' => (int) _whami_map_toolkit_conf_variable_value('MAX_RESULTS', 5),
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
  );

  $form[$module_name . '_' . $source_id . '_' . 'MAX_REQUEST'] = array(
    '#type' => 'textfield',
    '#title' => t('Search limit'),
    '#description' => t('The max. amount of places to request from one source'),
    '#default_value' => (int) _whami_map_toolkit_conf_variable_value('MAX_REQUEST', 5),
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => TRUE,
  );

  $data_sources = whami_build_data_sources();
  if ( count($data_sources) ) {
//    $enabled_sources = variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'sources'), '');
//    $enabled_sources = explode('+', $enabled_sources);

    $form['sources'] = array(
      '#type' => 'fieldset',
      '#title' => t('whami data sources to request data from'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    foreach ($data_sources as $data_source => $value) {
      $form['sources'][$module_name . '_' . $source_id . '_' . 'SOURCES' . '_' . $data_source] = array(
        '#type' => 'checkbox',
        '#title' => t('%name', array('%name' => $data_source)),
        '#description' => t('Get data from %name', array('%name' => $data_source)),
        '#default_value' => _whami_map_toolkit_conf_variable_value('SOURCES' . '_' . $data_source, '', $source_id),
      );
    }
  }

  return system_settings_form($form);
}

/**
 * Get the source-specific name of a configuration option
 *
 * @param string $source_id
 *   Source Adapter id
 * @param string $block_name
 *   Name of the rendered block
 * @param string $option_name
 *   Option
 * @param string $suffix
 *   Suffix
 */
function _whami_map_toolkit_conf_variable_name($option_name, $source_id = 'TOOLKIT', $module_name = 'WHAMI_MAP') {
  $ret = $module_name . '_' . $source_id . '_' . $option_name;

  return $ret;
}

function _whami_map_toolkit_conf_variable_value($option_name, $default = '', $source_id = 'TOOLKIT', $module_name = 'WHAMI_MAP') {
  return variable_get(
    _whami_map_toolkit_conf_variable_name($option_name, $source_id, $module_name),
    $default
    );
}

function _whami_map_toolkit_get_enabled_content_types() {

  $enabled_types = array();

  $content_types = _whami_map_toolkit_get_options_content_types();
  foreach ($content_types as $key => $type) {
    if(_whami_map_toolkit_conf_variable_value('CONTENT_TYPE' . '_' . $type, FALSE)) {
      $enabled_types[$key] = $type;
    }
  }

  return $enabled_types;
}

function _whami_map_toolkit_get_enabled_sources() {

  $enabled_sources = array();

  $data_sources = whami_build_data_sources();
  foreach ($data_sources as $source_id => $source) {
    if(_whami_map_toolkit_conf_variable_value('SOURCES' . '_' . $source_id, FALSE)) {
      $enabled_sources[] = $source_id;
    }
  }

  return $enabled_sources;
}

function _whami_map_toolkit_get_options_content_types() {
  $ret = array();

  $content_types = node_get_types();
  if ( !empty($content_types) ) {
    foreach($content_types as $type => $obj) {
      $ret[$type] = $obj->name;
    }
  }

  return $ret;
}

function _whami_map_toolkit_get_options_fields($type) {
  $ret = array();

  if ( module_exists('content') && function_exists('content_fields') ) {
    $fields = content_fields();
    if ( !empty($fields) ) {
      foreach($fields as $field_name => $field) {
        if ( !strcmp($field['type'], $type) && $field['active'] ) {
          $ret[$field_name] = $field['widget']['label'];
        }
      }
    }
  }

  return $ret;
}