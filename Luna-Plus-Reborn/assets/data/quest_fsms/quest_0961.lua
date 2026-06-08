--[[
  Quest 961: Soundless Sob
  Level Required: 110
  NPC Start: 543, NPC Complete: 544
  Rewards: EXP=20095498, Gold=106426
]]

local fsm = require('fsm_engine')

local quest_961 = fsm:new({
    id = 961,
    name = "Soundless Sob",
    level_required = 110,
    npc_start = 543,
    npc_complete = 544,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 961: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 961: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 961: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 326,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 961: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 961: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 543,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 961: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 961: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 544,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 961: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 961: Rewards given")
                self:give_rewards({
                    exp = 20095498,
                    gold = 106426,
                })
            end,
            transitions = {},
        },
    },
})

return quest_961