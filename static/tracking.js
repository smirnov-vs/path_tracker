ymaps.ready(() => {
    const myMap = new ymaps.Map("map", {
        center: [55.76, 37.64],
        zoom: 7
    });

    const points = [
        [59.94, 30.31], [55.4, 34.6], [55.4, 39.6]
    ];

    const polyline = new ymaps.Polyline(points);

    myMap.geoObjects.add(polyline);
    myMap.setBounds(polyline.geometry.getBounds());

    const myCollection = new ymaps.GeoObjectCollection();
    points.forEach((point) => {
        myCollection.add(new ymaps.Placemark(point));
    });
    myMap.geoObjects.add(myCollection);
});