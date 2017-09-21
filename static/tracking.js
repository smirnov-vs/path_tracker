ymaps.ready(() => {
    const map = new ymaps.Map("map", {
        center: [55.76, 37.64],
        zoom: 7
    });

    $.getJSON("track?id=222").done((data) => {
        const myCollection = new ymaps.GeoObjectCollection();
        const points = [];
        data.forEach((item) => {
            const point = [item.latitude, item.longitude];
            myCollection.add(new ymaps.Placemark(point, {
                hintContent: item.latitude + ", " + item.longitude,
                balloonContent: 'Время: ' + item.time
            }));
            myCollection.add(new ymaps.GeoObject({
                geometry: {
                    type: "Circle",
                    coordinates: point,
                    radius: item.accuracy
                }
            }));
            points.push(point);
        });

        if (points.length > 0) {
            const polyline = new ymaps.Polyline(points);
            map.geoObjects.add(myCollection);
            map.geoObjects.add(polyline);
            map.setBounds(polyline.geometry.getBounds());
        }
    }).fail(() => {
        Materialize.toast('Произошла ошибка при загрузке данных', 10000, 'rounded');
    });
});