--[[
  Quest 222: [Party] Threat at The Gate of Alker
  Level Required: 7
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=8950, Gold=3165
]]

local fsm = require('fsm_engine')

local quest_222 = fsm:new({
    id = 222,
    name = "[Party] Threat at The Gate of Alker",
    level_required = 7,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 222: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 222: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 222: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 128,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 222: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 129,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 222: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 130,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 222: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 131,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 222: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 222: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 222: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 222: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 222: Rewards given")
                self:give_rewards({
                    exp = 8950,
                    gold = 3165,
                    items = {
                        { item_id = 21000006, count = 5 },
                        { item_id = 21001524, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_222