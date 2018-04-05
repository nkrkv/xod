open Belt;

type t = Js.Types.obj_val;

type path = PatchPath.t;

[@bs.module "xod-project"] external create : unit => t = "createPatch";

[@bs.module "xod-project"] external getPath : t => path = "getPatchPath";

[@bs.module "xod-project"]
external _assocNode : (Node.t, t) => t = "assocNode";

let assocNode = (patch, node) => _assocNode(node, patch);

[@bs.module "xod-project"]
external _assocLink : (Link.t, t) => Either.t(Js.Exn.t, t) = "assocLink";

let assocLink = (patch, link) => _assocLink(link, patch) |> Either.toResult;

let assocLinkExn = (link, patch) =>
  switch (assocLink(link, patch)) {
  | Ok(patch') => patch'
  | Error(err) =>
    /* TODO: use a rerise mechanism */
    Js.Exn.raiseError(err |> Js.Exn.message |. Option.getWithDefault(""))
  };

[@bs.module "xod-project"]
external _listPins : t => array(Pin.t) = "listPins";

let listPins = patch => _listPins(patch) |. List.fromArray;

let listInputPins = patch =>
  patch |. listPins |. List.keep(pin => Pin.getDirection(pin) == Pin.Input);

let listOutputPins = patch =>
  patch |. listPins |. List.keep(pin => Pin.getDirection(pin) == Pin.Output);

/* TODO: is it defined anywhere already? */
let identity = a => a;

let findPinByLabel = (patch, label, ~normalize, ~direction) : option(Pin.t) =>
  listPins(patch)
  |. (normalize ? Pin.normalizeLabels : identity)
  |. List.keep(pin => Pin.getLabel(pin) == label)
  |. (
    pins =>
      switch (direction) {
      | None => pins
      | Some(dir) => List.keep(pins, pin => Pin.getDirection(pin) == dir)
      }
  )
  |. (
    pins =>
      switch (pins) {
      | [onlyPin] => Some(onlyPin)
      | _ => None
      }
  );

[@bs.module "xod-project"]
external _getAttachments : t => array(Attachment.t) = "getPatchAttachments";

let getAttachments = t => _getAttachments(t) |. List.fromArray;

let getTabtestContent = t =>
  getAttachments(t)
  |. List.keep(att => Attachment.getFilename(att) == "patch.test.tsv")
  |. List.head
  |. Option.map(Attachment.getContent);
