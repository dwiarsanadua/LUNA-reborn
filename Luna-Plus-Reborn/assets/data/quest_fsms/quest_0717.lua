--[[
  Quest 717: [Party] Pulling Strings?
  Level Required: 0
  NPC Start: 46, NPC Complete: 46
  Prerequisites: [712]
  Rewards: EXP=617324
]]

local fsm = require('fsm_engine')

local quest_717 = fsm:new({
    id = 717,
    name = "[Party] Pulling Strings?",
    level_required = 0,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {712},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 717: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 717: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 717: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 152,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 717: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 372,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 717: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 717: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 717: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 717: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 717: Rewards given")
                self:give_rewards({
                    exp = 617324,
                })
            end,
            transitions = {},
        },
    },
})

return quest_717