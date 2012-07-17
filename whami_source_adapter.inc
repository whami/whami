<?php
/**
 * @file
 * Functions to manage the module configuration
 */

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
function _whami_source_adapter_conf_variable_name($source_id, $block_name, $option_name, $suffix = '') {
  $ret = 'whami_source_adapter_' . $source_id . '_block_' . $block_name . '_' . $option_name;

  return drupal_strlen($suffix) ? $ret . '_' . $suffix : $ret;
}

/**
 * Generate the block config form
 *
 * @param string $source_id
 *   Source Adapter id
 * @param string $delta
 *   Block delta
 */
function _whami_source_adapter_block_config_form($source_id, $delta, $whami_content_fields = '') {
  $form = array();

  if(!is_array($whami_content_fields)) $whami_content_fields = array('geofield', 'sources');

  if ( is_numeric(array_search('sources', $whami_content_fields)) ) {
          $data_sources = whami_build_data_sources();
  }

  if (drupal_strlen($source_id) && drupal_strlen($delta) && module_exists('content') && function_exists('content_types')) {
    $types = content_types();
    if (!empty($types)) {
      $form['whami_source_adapter_' . $source_id] = array(
        '#type' => 'fieldset',
        '#title' => t('Enabled content types'),
        '#description' => t('Enable the content types on which this module will become active'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
      );

      foreach ($types as $type_name => $type) {
        $form['whami_source_adapter_' . $source_id][$type_name] = array(
          '#type' => 'fieldset',
          '#title' => t('Content type: %name', array('%name' => $type_name)),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );

        $form['whami_source_adapter_' . $source_id][$type_name]['value'] = array(
          '#type' => 'checkbox',
          '#title' => t('Enable %name', array('%name' => $type_name)),
          '#description' => t('Enable this content type for this module'),
          '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'active'), 0),
        );

        if (function_exists('content_fields')) {
          $cckfields = content_fields(NULL, $type_name);
          unset($geofields);
          unset($linkfields);
          foreach ($cckfields as $field_name => $field) {

            //Check if cck geo field
            if (!strcmp($field['type'], 'geo') && $field['widget_active'] && is_numeric(array_search('geofield', $whami_content_fields)) ) {
              $geofields[$field_name] = $field['widget']['label'];
            }

            //Check if cck link field
            if (!strcmp($field['type'], 'link') && $field['widget_active'] && is_numeric(array_search('linkfield', $whami_content_fields)) ) {
              $linkfields[$field_name] = $field['widget']['label'];
            }

            //Check if cck text field
            if (!strcmp($field['type'], 'text') && $field['widget_active'] && is_numeric(array_search('textfield', $whami_content_fields)) ) {
              $textfields[$field_name] = $field['widget']['label'];
            }

          } //endloop over cckfields

          if (!empty($geofields)) {
            $form['whami_source_adapter_' . $source_id][$type_name]['geofield'] = array(
              '#type' => 'select',
              '#title' => t('Select a geo field'),
              '#options' => $geofields,
              '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'geofield'), ''),
              '#description' => t('The Geo field with the coordinates'),
            );

            $form['whami_source_adapter_' . $source_id][$type_name]['radius'] = array(
              '#type' => 'textfield',
              '#title' => t('Set a radius'),
              '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'radius'), 10),
              '#description' => t('The radius for the proximity search'),
              '#size' => 3,
              '#maxlength' => 3,
            );

            $form['whami_source_adapter_' . $source_id][$type_name]['max_results'] = array(
              '#type' => 'textfield',
              '#title' => t('max. results'),
              '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'max_results'), 10),
              '#description' => t('The max. number of total results to get from the underlying service'),
              '#size' => 3,
              '#maxlength' => 3,
            );
          }

        if(!empty($linkfields)) {
            $form['whami_source_adapter_' . $source_id][$type_name]['linkfield'] = array(
              '#type' => 'select',
              '#title' => t('Select a link field'),
              '#options' => $linkfields,
              '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'linkfield'), ''),
              '#multiple' => FALSE,
              '#required' => TRUE,
            );
          }

          if(!empty($textfields)) {
            $form['whami_source_adapter_' . $source_id][$type_name]['textfield'] = array(
              '#type' => 'select',
              '#title' => t('Select a text field'),
              '#options' => $textfields,
              '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'textfield'), ''),
              '#multiple' => FALSE,
              '#required' => TRUE,
            );
          }

        } //endif function_exists('content_fields')


        if ( count($data_sources) ) {

          $enabled_sources = variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'sources'), 0);
          $enabled_sources = explode('+', $enabled_sources);

          $form['whami_source_adapter_' . $source_id][$type_name]['sources'] = array(
            '#type' => 'fieldset',
            '#title' => t('whami data sources to request data from'),
            '#collapsible' => TRUE,
            '#collapsed' => FALSE,
          );

          foreach ($data_sources as $data_source => $value) {
            $form['whami_source_adapter_' . $source_id][$type_name]['sources'][$data_source] = array(
              '#type' => 'checkbox',
              '#title' => t('%name', array('%name' => $data_source)),
              '#description' => t('Get data from %name', array('%name' => $data_source)),
              '#default_value' => is_numeric(array_search($data_source, $enabled_sources)),
            );
          }

          $form['whami_source_adapter_' . $source_id][$type_name]['max_request_results'] = array(
            '#type' => 'textfield',
            '#title' => t('max. request results'),
            '#default_value' => variable_get(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'max_request_results'), 10),
            '#description' => t('The max. number of results for one single request from the underlying service'),
            '#size' => 3,
            '#maxlength' => 3,
          );

        }

      } //end loop over content types
    }
  }

  return $form;
}

/**
 * Save the block config form
 *
 * @param string $source_id
 *   Source Adapter id
 * @param string $delta
 *   Block delta
 * @param array $edit
 *   Block edit
 */
function _whami_source_adapter_block_config_save($source_id, $delta, $edit = array()) {
  if (!empty($edit['whami_source_adapter_' . $source_id])) {
    $conf = $edit['whami_source_adapter_' . $source_id];
    foreach ($conf as $type_name => $type) {
      variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'active'), $type['value']);

      if (isset($type['geofield']) && !empty($type['geofield'])) {
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'geofield'), $type['geofield']);
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'radius'), $type['radius']);
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'max_results'), $type['max_results']);
      }
      if (isset($type['linkfield']) && !empty($type['linkfield'])) {
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'linkfield'), $type['linkfield']);
      }

      if (isset($type['textfield']) && !empty($type['textfield'])) {
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'textfield'), $type['textfield']);
      }

      $sources = array();
      if (isset($type['sources'])) {
        foreach ($type['sources'] as $source => $data_source) {
          if($type['sources'][$source]) $sources[$source] = $source;
        }
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'sources'), implode('+', $sources));
        variable_set(_whami_source_adapter_conf_variable_name($source_id, $delta, $type_name, 'max_request_results'), $type['max_request_results']);
        $sources = array();
      }

    }
  }
}
