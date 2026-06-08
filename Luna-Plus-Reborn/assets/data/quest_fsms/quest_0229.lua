--[[
  Quest 229: [Party] Continuing Threat: Kytan
  Level Required: 30
  NPC Start: 64, NPC Complete: 65
  Prerequisites: [224]
  Rewards: EXP=37033, Gold=6982
]]

local fsm = require('fsm_engine')

local quest_229 = fsm:new({
    id = 229,
    name = "[Party] Continuing Threat: Kytan",
    level_required = 30,
    npc_start = 64,
    npc_complete = 65,
    prerequisites = {224},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 229: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 229: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 229: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 138,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 229: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 11,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 229: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 229: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 229: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 229: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 229: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 229: Rewards given")
                self:give_rewards({
                    exp = 37033,
                    gold = 6982,
                    items = {
                        { item_id = 149, count = 1 },
                        { item_id = 149, count = 1 },
                        { item_id = 153, count = 1 },
                        { item_id = 153, count = 1 },
                        { item_id = 21000095, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_229