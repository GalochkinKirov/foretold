open Utils;
type measurable = DataModel.Measurable.t;
let graph = Data.make;

let itemUrl = id => DataModel.Url.toString(ItemShow(id));

let findName = propertyId =>
  graph
  |> Graph_T.F.factList
  |> Graph_Fact_Filters.withSubject(propertyId)
  |> Graph_Fact_Filters.withProperty("@base/properties/p-name")
  |> E.L.head
  |> E.O.bind(_, (k: Graph_T.T.fact) =>
       switch (k.value.valueType) {
       | String(s) => Some(s)
       | ThingId(s) => Some(s)
       | _ => None
       }
     );

let names = subjectId =>
  graph
  |> Graph_T.F.factList
  |> Graph_Fact_Filters.withSubject(subjectId)
  |> E.L.fmap((f: Graph_T.T.fact) => f);