--[[
  Quest 167: Lizardmen's Fury
  Level Required: 37
  NPC Start: 33, NPC Complete: 33
  Prerequisites: [166]
  Rewards: EXP=69077, Gold=6700
]]

local fsm = require('fsm_engine')

local quest_167 = fsm:new({
    id = 167,
    name = "Lizardmen's Fury",
    level_required = 37,
    npc_start = 33,
    npc_complete = 33,
    prerequisites = {166},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 167: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 167: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 167: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 58,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 167: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 167: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 167: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 33,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 167: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 167: Rewards given")
                self:give_rewards({
                    exp = 69077,
                    gold = 6700,
                    items = {
                        { item_id = 30000179, count = 2 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_167