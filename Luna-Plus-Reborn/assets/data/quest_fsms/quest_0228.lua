--[[
  Quest 228: [Party] Continuing Threat: Karas
  Level Required: 30
  NPC Start: 64, NPC Complete: 65
  Prerequisites: [224]
  Rewards: EXP=37033, Gold=6803
]]

local fsm = require('fsm_engine')

local quest_228 = fsm:new({
    id = 228,
    name = "[Party] Continuing Threat: Karas",
    level_required = 30,
    npc_start = 64,
    npc_complete = 65,
    prerequisites = {224},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 228: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 228: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 228: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 137,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 228: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 53,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 228: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 228: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 228: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 228: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 65,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 228: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 228: Rewards given")
                self:give_rewards({
                    exp = 37033,
                    gold = 6803,
                    items = {
                        { item_id = 148, count = 1 },
                        { item_id = 148, count = 1 },
                        { item_id = 152, count = 1 },
                        { item_id = 152, count = 1 },
                        { item_id = 21000514, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_228