<?php
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_CONTENT_TYPE', 'whami_source_adapter_facebook_content_type');
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_FIELD_GEO', 'whami_source_adapter_facebook_field_geo');
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_SEARCH_RADIUS', 'whami_source_adapter_facebook_search_radius');
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_SEARCH_LIMIT', 'whami_source_adapter_facebook_search_limit');
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_DEBUG', 'debug');
define('WHAMI_SOURCE_ADAPTER_FACEBOOK_TMP_TOKEN', 'tmp_token');

function _whami_source_adapter_facebook_admin() {

  $form = array();

  $form[_whami_source_adapter_facebook_get_user_setting(WHAMI_SOURCE_ADAPTER_FACEBOOK_DEBUG)] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable debug mode'),
      '#default_value' => _whami_source_adapter_facebook_debug(),
  );

  $tmp_fb_token = _whami_source_adapter_facebook_tmp_token();

  $form[_whami_source_adapter_facebook_get_user_setting(WHAMI_SOURCE_ADAPTER_FACEBOOK_TMP_TOKEN)] = array(
    '#type' => 'textfield',
    '#title' => t('Access token'),
    '#description' => t('Input an access token to requested data from Facebook.
                         Unless you really know what you are doing, this should be empty.'),
    '#default_value' => $tmp_fb_token,
    '#size' => 200,
    '#maxlength' => 200,
  );

  if(strlen($tmp_fb_token)) {
    $form['whami_facebook_token_custom'] = array(
      '#type' => 'item',
      '#title' => t('User defined Token infos'),
      '#value' => whami_facebook_get_fb_token_infos($tmp_fb_token),
    );
  }

  $fb_app_token = whami_source_facebook_get_fb_token('app');
  $form['whami_facebook_token_app'] = array(
    '#type' => 'item',
    '#title' => t('App Token'),
    '#value' => $fb_app_token,
  );

  $form['whami_facebook_token_app_infos'] = array(
    '#type' => 'item',
    '#title' => t('App Token Infos'),
    '#value' => whami_facebook_get_fb_token_infos($fb_app_token, 'app'),
  );

  $fb_user_token = whami_source_facebook_get_fb_token('user');
  $form['whami_facebook_token_user'] = array(
    '#type' => 'item',
    '#title' => t('User Token'),
    '#value' => $fb_user_token,
  );

  $form['whami_facebook_token_user_infos'] = array(
    '#type' => 'item',
    '#title' => t('User Token Infos'),
    '#value' => whami_facebook_get_fb_token_infos($fb_user_token, 'user'),
  );

  whami_print_debug_log();

  return system_settings_form($form);

}

function _whami_source_adapter_facebook_content_type_enabled($type) {
  return variable_get(WHAMI_SOURCE_ADAPTER_FACEBOOK_CONTENT_TYPE . '_' . $type, FALSE);
}

function _whami_source_adapter_facebook_get_field_geo() {
  return variable_get(WHAMI_SOURCE_ADAPTER_FACEBOOK_FIELD_GEO, '');
}

function _whami_source_adapter_facebook_search_radius() {
  return variable_get(WHAMI_SOURCE_ADAPTER_FACEBOOK_SEARCH_RADIUS, 1000);
}

function _whami_source_adapter_facebook_search_limit() {
  return variable_get(WHAMI_SOURCE_ADAPTER_FACEBOOK_SEARCH_LIMIT, 50);
}

function _whami_source_adapter_facebook_debug() {
  return variable_get(
     _whami_source_adapter_facebook_get_user_setting(WHAMI_SOURCE_ADAPTER_FACEBOOK_DEBUG), FALSE
  );
}

function _whami_source_adapter_facebook_tmp_token() {
  return variable_get(
    _whami_source_adapter_facebook_get_user_setting(WHAMI_SOURCE_ADAPTER_FACEBOOK_TMP_TOKEN), ''
  );
}

function _whami_source_adapter_facebook_get_options_content_types() {

  $ret = array();

  $content_types = node_get_types();
  if (!empty($content_types)) {
    foreach($content_types as $type => $obj) {
      $ret[$type] = $obj->name;
    }
  }

  return $ret;

}

function _whami_source_adapter_facebook_get_options_fields($type) {
  $ret = array();
  if (module_exists('content') && function_exists('content_fields')) {
    $fields = content_fields();
    if (!empty($fields)) {
      foreach($fields as $field_name => $field) {
        if ( !strcmp($field['type'], $type) && $field['active'] ) {
          $ret[$field_name] = $field['widget']['label'];
        }
      }
    }
  }
  return $ret;
}

function _whami_source_adapter_facebook_get_user_setting($variable_name, $uid = false) {
  $uid = $uid ? $uid : $GLOBALS['user']->uid;
  return   'whami_source_adapter_facebook' . '-uid-' . $uid . '_' . $variable_name;
}