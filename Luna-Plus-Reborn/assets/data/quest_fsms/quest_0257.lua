--[[
  Quest 257: The broken power generator
  Level Required: 150
  NPC Start: 22, NPC Complete: 22
]]

local fsm = require('fsm_engine')

local quest_257 = fsm:new({
    id = 257,
    name = "The broken power generator",
    level_required = 150,
    npc_start = 22,
    npc_complete = 22,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 257: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 257: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 257: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 52,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 257: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 257: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 257: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 257: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 257: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 22,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 257: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 257: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000146, count = 1 },
                        { item_id = 21000147, count = 1 },
                        { item_id = 30000465, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_257