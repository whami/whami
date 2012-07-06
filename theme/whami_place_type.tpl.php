<?php

if(!strlen($whami_place_type) && !strlen($whami_place_icon)) return;
?>

<table>
  <tr>
    <td>
      <?php if ($whami_place_type) : ?>
        <?php print $whami_place_type; ?>
      <?php endif;?>

      <?php if($whami_place_icon) : ?>
        on
        <?php print $whami_place_icon; ?>
      <?php endif;?>
    </td>
  </tr>
</table>