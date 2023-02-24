<body>
    <h1>FarmBot</h1>
    <div class="container"> 
        <a href="/liveview">Live View</a>
        <a href="/manpan">Management Panel</a>
        <a href="/graph">Data Graphs</a> 
    </div>
    
</body>
<style>
    body {
        
        font-family: Arial, Helvetica, sans-serif;
        text-align: center;
    }
    h1 {
        font-size: 50px;
    }
    .container {
        display: grid;
        grid-template-columns: .1fr .3fr .1fr;
        grid-template-rows: auto;
        grid-gap: 10px;
        justify-content: space-evenly;        
    }
    a {
        font-size: 30px;
        text-decoration: none;
        color: black;

    }
</style>