-- pm-declare-name: Crease Smoothing
-- pm-declare-input: creaseAngle | crease angle | double | val = 10 ; min = 0 ; max = 360
-- pm-declare-input: sharpRefinements | sharp refinements | integer | val = 2
-- pm-declare-input: smoothRefinements | smooth refinements | integer | val = 2
-- pm-declare-input: strictSubsetInheritance | strict subset inheritance | boolean | val = false

SelectAll(mesh)
ConvertToTriangles(mesh)

for i = 1, sharpRefinements do
	ClearSelection(mesh)
	SelectCreaseEdges(mesh, creaseAngle)
	ExtendSelection(mesh, 1)
	Refine(mesh, strictSubsetInheritance)
end

for i = 1, smoothRefinements do
	ClearSelection(mesh)
	SelectCreaseEdges(mesh, creaseAngle)
	ExtendSelection(mesh, 1)
	ConvertToTriangles(mesh)
	RefineSmooth(mesh, strictSubsetInheritance)
end

SelectAll(mesh)
ConvertToTriangles(mesh)

ClearSelection(mesh)
