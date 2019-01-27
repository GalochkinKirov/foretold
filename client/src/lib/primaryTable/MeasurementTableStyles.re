open Utils;
open Rationale;
open Css;
let stringOfFloat = Js.Float.toPrecisionWithPrecision(_, ~digits=3);

let middle =
  style([
    textAlign(`center),
    fontSize(`em(1.4)),
    fontWeight(800),
    color(`hex("7d7ea2")),
    marginTop(`px(6)),
  ]);
let area = style([maxHeight(`px(50))]);

let isJudgement = (m: MeasurableTypes.measurement) =>
  m##competitorType == `OBJECTIVE;

let group =
  style([borderRadius(`px(2)), border(`px(1), `solid, `hex("e1eaf1"))]);
let row = (~m: MeasurableTypes.measurement) => {
  let isJudge = m##competitorType == `OBJECTIVE;
  style([
    width(`percent(100.0)),
    selector(" h2", [marginTop(px(2))]),
    display(`flex),
    flexDirection(`row),
    backgroundColor(`hex(isJudge ? "eaf0f5" : "eaf0f5")),
    selector(":last-child", [borderBottom(`px(0), `solid, hex("fff"))]),
  ]);
};
/* e3e7f5 */
let mainColumn = style([flex(1), display(`flex), flexDirection(`column)]);

let item =
  style([
    float(`left),
    fontSize(`px(14)),
    marginRight(`px(18)),
    color(`hex("red")),
  ]);

let rightColumn = (~m: MeasurableTypes.measurement) => {
  let isJudge = isJudgement(m);
  style([
    flex(3),
    display(`flex),
    backgroundColor(`hex(isJudge ? "f3f1f5" : "f3f5f7")),
    borderBottom(`px(1), `solid, `hex(isJudge ? "e5dfec" : "e8eef3")),
    borderTopRightRadius(`px(2)),
    borderBottomRightRadius(`px(2)),
  ]);
};

let rightColumnInner = style([flex(1), padding2(~v=`px(4), ~h=`px(10))]);

let mainColumnTop = style([flex(1), paddingLeft(px(2))]);

let rightColumnTop = style([flex(1), paddingLeft(px(2))]);

let mainColumnBottom = style([flex(1), padding(px(2))]);

let smallDistribution = (m: MeasurableTypes.measurement, g: (float, float)) =>
  switch (m##value) {
  | Belt.Result.Ok(`FloatCdf(r)) =>
    r
    |> Value.toPdf(~bucketSize=20)
    |> Value.FloatCdf.toPoints
    |> (data => <WideChart data bounds=g />)
  | Belt.Result.Ok(`FloatPoint(r)) =>
    <div className=middle> {stringOfFloat(r) |> ste} </div>
  | _ => "" |> ste
  };

let agentStyle =
  style([color(`rgb((102, 121, 134))), fontSize(`em(1.1))]);

let judgementStyle =
  style([
    selector(
      " h3",
      [
        color(`rgba((55, 47, 68, 0.85))),
        marginBottom(`px(0)),
        fontSize(`em(1.15)),
        fontWeight(800),
      ],
    ),
    selector(" a", [fontSize(`em(0.9))]),
  ]);

let agentLink = (~m: MeasurableTypes.measurement) => {
  open Rationale.Option.Infix;
  let agent = m##agent;
  let aLink =
    switch (agent <$> (x => x##id), agent >>= (x => x##name)) {
    | (Some(id), Some(name)) =>
      <a href={"/agents/" ++ id} className=agentStyle> {name |> ste} </a>
    | (_, _) => "" |> ste
    };
  let isJudge = isJudgement(m);
  if (isJudge) {
    <div className=judgementStyle>
      <h3> {"Judgement" |> ste} </h3>
      <span> {"by " |> ste} </span>
      aLink
    </div>;
  } else {
    aLink;
  };
};

let descriptionStyle =
  style([
    marginTop(`px(5)),
    selector(
      " p",
      [color(`rgba((80, 80, 80, 0.78))), marginBottom(`px(0))],
    ),
  ]);

let description = (~m: MeasurableTypes.measurement) =>
  switch (m##description |> Option.default("")) {
  | "" => <div />
  | text => <div className=descriptionStyle> <p> {text |> ste} </p> </div>
  };

let formatDate = e =>
  Option.Infix.(e <$> MomentRe.Moment.format("L") |> Option.default(""));

let dateStyle =
  style([
    float(`right),
    color(`rgba((35, 35, 35, 0.34))),
    marginTop(`px(1)),
  ]);

let relevantAt = (~m: MeasurableTypes.measurement) =>
  <div className=dateStyle> {m##relevantAt |> formatDate |> ste} </div>;