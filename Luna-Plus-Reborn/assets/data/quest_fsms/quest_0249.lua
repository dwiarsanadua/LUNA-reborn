--[[
  Quest 249: The Gryphon Statue
  Level Required: 43
  NPC Start: 66, NPC Complete: 66
  Prerequisites: [248]
  Rewards: EXP=118872, Gold=12000
]]

local fsm = require('fsm_engine')

local quest_249 = fsm:new({
    id = 249,
    name = "The Gryphon Statue",
    level_required = 43,
    npc_start = 66,
    npc_complete = 66,
    prerequisites = {248},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 249: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 249: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 249: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 249: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 249: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 249: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 249: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 66,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 249: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 249: Rewards given")
                self:give_rewards({
                    exp = 118872,
                    gold = 12000,
                    items = {
                        { item_id = 160, count = 1 },
                        { item_id = 161, count = 1 },
                        { item_id = 160, count = 1 },
                        { item_id = 30000433, count = 1 },
                        { item_id = 30000436, count = 20 },
                        { item_id = 161, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_249