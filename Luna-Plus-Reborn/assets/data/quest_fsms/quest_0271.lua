--[[
  Quest 271: Fighter Faction Armor Materials
  Level Required: 150
  NPC Start: 57, NPC Complete: 57
  Prerequisites: [251]
]]

local fsm = require('fsm_engine')

local quest_271 = fsm:new({
    id = 271,
    name = "Fighter Faction Armor Materials",
    level_required = 150,
    npc_start = 57,
    npc_complete = 57,
    prerequisites = {251},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 271: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 271: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 271: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 271: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 271: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 271: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 271: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000148, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_271