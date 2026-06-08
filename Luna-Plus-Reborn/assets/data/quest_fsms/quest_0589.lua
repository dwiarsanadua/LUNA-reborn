--[[
  Quest 589: Plucking Ghost Flowers
  Level Required: 0
  NPC Start: 300, NPC Complete: 300
  Prerequisites: [588]
]]

local fsm = require('fsm_engine')

local quest_589 = fsm:new({
    id = 589,
    name = "Plucking Ghost Flowers",
    level_required = 0,
    npc_start = 300,
    npc_complete = 300,
    prerequisites = {588},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 589: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 589: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 589: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 2,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 589: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 589: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 589: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 300,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 589: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 589: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001095, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_589