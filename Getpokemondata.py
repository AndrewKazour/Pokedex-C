import requests
import csv

def fetch_pokemon_data():
    pokemon_data = []
    
    for i in range(1, 152): 
        url = f"https://pokeapi.co/api/v2/pokemon/{i}"
        response = requests.get(url)
        data = response.json()
        
        # Get species data for description
        species_url = data['species']['url']
        species_data = requests.get(species_url).json()
        
        # Extract English description
        descriptions = species_data['flavor_text_entries']
        description = next((d['flavor_text'] for d in descriptions 
                          if d['language']['name'] == 'en'), "")
        description = description.replace('\n', ' ').replace('\f', ' ')
        
        # Extract data
        pokemon = {
            'id': data['id'],
            'name': data['name'].capitalize(),
            'type1': data['types'][0]['type']['name'].capitalize(),
            'type2': data['types'][1]['type']['name'].capitalize() if len(data['types']) > 1 else '',
            'hp': data['stats'][0]['base_stat'],
            'attack': data['stats'][1]['base_stat'],
            'defense': data['stats'][2]['base_stat'],
            'sp_attack': data['stats'][3]['base_stat'],
            'sp_defense': data['stats'][4]['base_stat'],
            'speed': data['stats'][5]['base_stat'],
            'ability1': data['abilities'][0]['ability']['name'].replace('-', ' ').title(),
            'ability2': data['abilities'][1]['ability']['name'].replace('-', ' ').title() if len(data['abilities']) > 1 else '',
            'description': description
        }
        
        pokemon_data.append(pokemon)
        print(f"Fetched #{i}: {pokemon['name']}")
    
    return pokemon_data

def save_to_csv(pokemon_data, filename='pokemon_data.csv'):
    fieldnames = ['id', 'name', 'type1', 'type2', 'hp', 'attack', 'defense', 
                  'sp_attack', 'sp_defense', 'speed', 'ability1', 'ability2', 'description']
    
    with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(pokemon_data)
    
    print(f"Saved to {filename}")

if __name__ == "__main__":
    data = fetch_pokemon_data()
    save_to_csv(data)