--[[
  Quest 273: Mage faction Armor Materials
  Level Required: 150
  NPC Start: 57, NPC Complete: 57
  Prerequisites: [251]
]]

local fsm = require('fsm_engine')

local quest_273 = fsm:new({
    id = 273,
    name = "Mage faction Armor Materials",
    level_required = 150,
    npc_start = 57,
    npc_complete = 57,
    prerequisites = {251},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 273: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 273: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 273: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 273: NPC talk objective met")
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
                self:log("Quest 273: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 273: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 273: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000164, count = 1 },
                        { item_id = 21000150, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_273