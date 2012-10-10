<?php
define('WHAMI_SOURCE_ADAPTER_DEBUG', 'whami_source_adapter_debug');

function _whami_source_adapter_admin() {
  $form = array();

  $form[WHAMI_SOURCE_ADAPTER_DEBUG . '-uid-' . $GLOBALS['user']->uid] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable debug mode'),
      '#default_value' => _whami_source_adapter_debug(),
  );

  return system_settings_form($form);
}

function _whami_source_adapter_debug() {
  return variable_get(WHAMI_SOURCE_ADAPTER_DEBUG . '-uid-' . $GLOBALS['user']->uid, FALSE);
}