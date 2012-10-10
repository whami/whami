<?php
/**
 * @file
 * Default theme implementation to display the licence information of a external place.
 *
 * Available variables:
 * - $whami_place: The place object.
 * - $whami_place_description: The licence text
 * - $action_link
 */

?>
<div class="whami-place whami-place-preview <?php print $whami_place['url_hash']; ?>">
  <table>

    <?php if($action_link) : ?>
      <tr><td class="whami-action-links">
        <span><b><?php print $action_link; ?></b></span>
      </td></tr>
    <?php endif;?>

    <?php if($whami_place_description) : ?>
      <tr><td>
        <?php print $whami_place_description; ?>
      </td></tr>
    <?php endif;?>

  </table>

</div>