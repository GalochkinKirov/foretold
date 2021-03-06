module Styles = {
  open Css;
  let dropdown =
    style([
      border(`px(1), `solid, `hex("d5d2d2")),
      padding2(~v=`em(0.5), ~h=`em(0.)),
      borderRadius(`px(5)),
      background(`hex("fff")),
      boxShadow(
        ~x=`px(1),
        ~y=`px(1),
        ~blur=`px(5),
        ~spread=`px(1),
        ~inset=false,
        `hex("dfd7d7"),
      ),
    ]);
  let action =
    style([
      cursor(`pointer),
      padding2(~v=`em(0.3), ~h=`em(1.4)),
      minWidth(`px(200)),
      fontSize(`em(1.2)),
      selector(":hover", [backgroundColor(`hex("eef0f3"))]),
    ]);
};