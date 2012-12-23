<?php
/**
 * @file
 * Default theme implementation to display the description of a place.
 *
 * Available variables:
 * - $whami_place: The place object.
 * - $whami_place_license:
 *
 */

?>
<div class="whami-place whami-place-description">
  <table>

    <?php if($whami_place_descr) : ?>
      <tr><td>
        <span><?php print $whami_place_descr; ?></span>
      </td></tr>
    <?php endif;?>

    <?php if($whami_place_license) : ?>
      <tr><td>
        <?php print $whami_place_license; ?>
      </td></tr>
    <?php endif;?>

  </table>

</div>