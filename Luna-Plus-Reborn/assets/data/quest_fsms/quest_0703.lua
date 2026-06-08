--[[
  Quest 703: Bounty Hunter
  Level Required: 48
  NPC Start: 40, NPC Complete: 40
  Prerequisites: [702]
  Rewards: EXP=239495
]]

local fsm = require('fsm_engine')

local quest_703 = fsm:new({
    id = 703,
    name = "Bounty Hunter",
    level_required = 48,
    npc_start = 40,
    npc_complete = 40,
    prerequisites = {702},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 703: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 703: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 703: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 703: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 703: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 703: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 703: Rewards given")
                self:give_rewards({
                    exp = 239495,
                    items = {
                        { item_id = 350, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_703