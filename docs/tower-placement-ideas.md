Currently:

- we are placing towers manually using the functions in grid (to get the position put in transform) and tower_placement

- but today it is hardcoded and you cant merge elements.


to change that, ideas:

- continue to use the tower transform to know when placing a tower if you are putting on the same place (using Grid::getnearestgridpointcentered)
- with that, you check if there is a tower there already. if there is, you just add to the elements a new element and put a resolve tag

- if not, you create a new tower entity, put the element and put the resolve tag

- then create a system for resolving the tags that will actually create the tower stats and such!