--[[
  Quest 252: Collecting Red Ore
  Level Required: 27
  NPC Start: 55, NPC Complete: 55
  Prerequisites: [251]
]]

local fsm = require('fsm_engine')

local quest_252 = fsm:new({
    id = 252,
    name = "Collecting Red Ore",
    level_required = 27,
    npc_start = 55,
    npc_complete = 55,
    prerequisites = {251},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 252: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 252: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 252: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 127,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 252: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 252: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 252: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 55,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 252: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 252: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 164, count = 10 },
                        { item_id = 21000164, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_252