<script lang="ts">
    let passphrase = '';
    let data = [,,,,]

    async function sendChange () {
        const res = await fetch('http://localhost:3000/update', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                passphrase,
                data
            })
        })
        const json = await res
        console.log(json)
    }
</script>
<br><br><br>
<div>
    passphrase <input bind:value={passphrase}/><br>
    pump on hour <input bind:value={data[0]}/><br>
    pump on min <input bind:value={data[1]}/><br>
    pump off min <input bind:value={data[2]}/><br>
    light on hour <input bind:value={data[3]}/><br>
    light off hour <input bind:value={data[4]}/><br>
</div>

<button type="button" on:click={sendChange}>
	Post it.
</button>