<?php
/**
 * @file
 * Default theme implementation to display the licence information of a external place.
 *
 * Available variables:
 * - $place: The place object
 * - $whami_ext_place_link: Themed "Facebook >" - link
 * - $whami_place_icon
 * - $whami_place['licence']
 */

?>
<div class="whami-place-licence" style='margin-top:0'>

  <?php if($whami_place_icon) : ?>
    <?php print $whami_place_icon; ?>
  <?php endif;?>

  <?php if($whami_ext_place_link) : ?>
    <?php print $whami_ext_place_link; ?>
  <?php endif;?>

  <?php if(strlen($whami_place['license'])) : ?>
    <div class="whami_read_license">
      <?php print $whami_place['license']; ?>
    </div>
  <?php endif;?>

</div>