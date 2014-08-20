#!/usr/local/bin/php
<?php

echo constrainBetween(10, 10, 11) == 10;  echo "\n";
echo constrainBetween(10, 11, 15) == 15;  echo "\n";
echo constrainBetween(10, 0, 20) == 10;   echo "\n";
echo constrainBetween(8, 10, 20) == 19;   echo "\n";
echo constrainBetween(12, 0, 10) == 1;    echo "\n";

function constrainBetween($value, $lower, $higher) {
  if ($value < $lower) {
    $value = $higher - ($lower - $value) + 1;
  } else if ($value > $higher) {
    $value = $lower + ($value - $higher) - 1;
  }
  return $value;
}

?>