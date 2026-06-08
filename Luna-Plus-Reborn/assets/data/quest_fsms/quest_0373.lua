--[[
  Quest 373: [A Class] Hiding in Plain Sight
  Level Required: 61
  NPC Start: 94, NPC Complete: 94
  Prerequisites: [372]
  Rewards: EXP=549177
]]

local fsm = require('fsm_engine')

local quest_373 = fsm:new({
    id = 373,
    name = "[A Class] Hiding in Plain Sight",
    level_required = 61,
    npc_start = 94,
    npc_complete = 94,
    prerequisites = {372},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 373: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 373: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 373: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 373: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 373: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 373: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 373: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 373: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 373: Rewards given")
                self:give_rewards({
                    exp = 549177,
                })
            end,
            transitions = {},
        },
    },
})

return quest_373