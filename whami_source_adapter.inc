<?php

/**
 * @file A collection of useful functions for the Whami Source Adapter and its
 * dependent modules
 */

/**
 *
 * Enter description here ...
 * @param unknown_type $source_id
 * @param unknown_type $block_name
 * @param unknown_type $option_name
 * @param unknown_type $suffix
 */
function _whami_source_adapter_conf_variable_name($source_id, $block_name, $option_name, $suffix = '') {
  $ret = 'whami_source_adapter_' . $source_id . '_block_' . $block_name . '_' . $option_name;

  return drupal_strlen($suffix) ? $ret . '_' . $suffix : $ret;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $source_id
 * @param unknown_type $delta
 */
function _whami_source_adapter_block_config_form($source_id, $delta) {
  $form = array();

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
          $geofields = content_fields(NULL, $type_name);
          foreach ($geofields as $field_name => $field) {
            if (!strcmp($field['type'], 'geo') && $field['widget_active']) {
              $geofields[$field_name] = $field['widget']['label'];
            }
            else {
              unset($geofields[$field_name]);
            }
          }

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
              '#description' => t('The max. number of results requested from the underlying service'),
              '#size' => 3,
              '#maxlength' => 3,
            );
          }
        }
      }
    }
  }

  return $form;
}

/**
 *
 * Enter description here ...
 * @param unknown_type $source_id
 * @param unknown_type $delta
 * @param unknown_type $edit
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
    }
  }
}