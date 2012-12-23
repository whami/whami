<?php
/**
 * @file
 * Default theme implementation to display the bubble of a external place.
 *
 * Available variables:
 * - $whami_place: The place object.
 * - $whami_place_teaser: The place teaser
 */

?>

<div class="whami-infoBubble-body">
  <?php if($whami_place_preview) : ?>
      <?php print $whami_place_preview; ?>
  <?php endif;?>
</div>