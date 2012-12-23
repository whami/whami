<?php
/**
 * @file
 * Default theme implementation to display a list of places.
 *
 * Available variables:
 * - $whami_places: The themed list of places
 *
 */
?>
<?php if($whami_places) : ?>
  <div class="whami-place-list">
    <?php print $whami_places; ?>
  </div>
<?php endif;?>