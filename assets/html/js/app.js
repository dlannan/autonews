var interval, settimeout, demo1, demo_filtering;

function process_url_embed(url) {

    url = url.replace("https://youtu.be/","https://www.youtube.com/embed/");
    url = url.replace("https://www.youtube.com/watch?v=","https://www.youtube.com/embed/");
    console.log(url)
    return url;
}


function render_items() {

    // Clear Previous Demo Cicle
    clearInterval(interval);
    clearInterval(settimeout);

    demo1.Change({
            dimensions: {
            width: "300",
            height: "auto",
            margin: "10",
            minHeight: "100", // if height is "random"
            maxHeight: "300"  // if height is "random"
            },
            style: {
            background: "random",
            borderRadius: "5"
            }
    });

    // Append Items
    {{#rows}}
        indata = '{{{data}}}';
        data = process_url_embed(indata);
        demo1.AddItem({
        items: '<div class="card" style="color:black; width: 100%;"><div class="card-img-top">'+data+'</div><div class="card-body"><p class="card-text">{{{text}}}</p></div></div>'
        });
    {{/rows}}
}

document.addEventListener("DOMContentLoaded", function() {
    demo1 = new EasyGrid({
        selector: "#grid",
        dimensions: {
            width: "150",
            height: "random",
            margin: "15",
            minHeight: "100",
            maxHeight: "300"
        },
        animations: {
            fadeInSpeed: "500"
        },
        style: {
            background: "random",
            borderRadius: "5"
        }
    });

    render_items();

    $('#search-google').click(function () { 
        console.log("[Search] " + $('#search-text').val());
        var query = $('#search-text').val();
        search_google(query);
    });

    $('#search-text').on("keyup", function(event){
        var keycode = (event.keyCode ? event.keyCode : event.which);
        if(keycode == '13'){
            console.log("[Search] " + $('#search-text').val());
            var query = $('#search-text').val();
            search_google(query);
        }
    });    
});

/* FIXED HEIGHT */
// document.getElementById("fixedheight").addEventListener("click", function() {

//     // Clear Previous Demo Cicle
//     clearInterval(interval);
//     clearInterval(settimeout);

//     demo1.Change({
//             dimensions: {
//             width: "150",
//             height: "270",
//             margin: "15",
//             minHeight: "100", // if height is "random"
//             maxHeight: "300"  // if height is "random"
//             },
//             style: {
//             background: "random",
//             borderRadius: "5"
//             }
//     });

//     // Append Items
//     interval = setInterval(function(){ 
//             demo1.AddItem({
//             items: ""
//             });

//     }, 100);  
//     // Clear Timeout  
//     settimeout = setTimeout(function(){ clearInterval(interval); }, 2000);  
// }); 

// /* BOOSTRAP CARDS */

// /* FEATURED POSTS */
// document.getElementById("festuredposts").addEventListener("click", function() {

//     // Clear Previous Demo Cicle
//     clearInterval(interval);
//     clearInterval(settimeout);

//     demo1.Change({
//             dimensions: {
//             width: "400",
//             height: "auto",
//             margin: "10",
//             minHeight: "100", // if height is "random"
//             maxHeight: "300"  // if height is "random"
//             },
//             style: {
//             background: "transparent",
//             borderRadius: "5"
//             }
//     });

//     // Append Items
//     interval = setInterval(function(){

//             demo1.AddItem({
//             items: '<div style="width:100%;" class="row no-gutters border rounded overflow-hidden flex-md-row mb-4 shadow-sm h-md-250 position-relative"> <div class="col p-4 d-flex flex-column position-static"> <strong class="d-inline-block mb-2 text-primary">World</strong> <h3 class="mb-0">Featured post</h3> <div class="mb-1 text-muted">Nov 12</div><p class="card-text mb-auto">This is a wider card with supporting text below as a natural lead-in to additional content.</p><a href="#" class="stretched-link">Continue reading</a> </div><div class="col-auto d-none d-lg-block"> <svg class="bd-placeholder-img" width="200" height="250" xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid slice" focusable="false" role="img" aria-label="Placeholder: Thumbnail"><title>Placeholder</title><rect width="100%" height="100%" fill="#55595c"></rect><text x="50%" y="50%" fill="#eceeef" dy=".3em">Thumbnail</text></svg> </div></div>'
//             });

//         }, 100); 

//     // Clear Timeout  
//     settimeout = setTimeout(function(){ clearInterval(interval); }, 1000);  
// }); 

// /* MOVIE CARDS */
// document.getElementById("moviecards").addEventListener("click", function() {

//     // Clear Previous Demo Cicle
//     clearInterval(interval);
//     clearInterval(settimeout);

//     demo1.Change({
//             dimensions: {
//             width: "200",
//             height: "auto",
//             margin: "10",
//             minHeight: "100", // if height is "random"
//             maxHeight: "300"  // if height is "random"
//             },
//             style: {
//             background: "random",
//             borderRadius: "5"
//             }
//     });

//     // Append Items
//     interval = setInterval(function(){

//             demo1.AddItem({
//             items: '<div style="color:black;" class="card movie_card"><img src="assets/html/images/200x270.png" class="card-img-top" alt="..."> <div class="card-body"> <i class="fas fa-play play_button" data-toggle="tooltip" data-placement="bottom" title="Play Trailer"></i> <h5 class="card-title" style="margin-left: -3px;">Movie Name</h5> <span class="movie_info">2019</span> <span class="movie_info float-right"><i class="fas fa-star"></i> <span style="font-weight:bold;">9</span> / 10</span></div> </div>'
//             });

//         }, 100); 

//     // Clear Timeout  
//     settimeout = setTimeout(function(){ clearInterval(interval); }, 1000);  
// }); 

// /* IMAGES TILES */
// document.getElementById("imagetiles").addEventListener("click", function() {

//     // Clear Previous Demo Cicle
//     clearInterval(interval);
//     clearInterval(settimeout);

//     demo1.Change({
//             dimensions: {
//             width: "200",
//             height: "auto",
//             margin: "10",
//             minHeight: "100", // if height is "random"
//             maxHeight: "300"  // if height is "random"
//             },
//             style: {
//             background: "transparent",
//             borderRadius: "5"
//             }
//     });

//     // Append Items
//     interval = setInterval(function(){ 

//             demo1.AddItem({
//             items: '<div style="text-align:center;"><img style="width:100%;" src="https://via.placeholder.com/200x270"></div>'
//             });

//     }, 100); 

//     // Clear Timeout  
//     settimeout = setTimeout(function(){ clearInterval(interval); }, 2000);  
// }); 

