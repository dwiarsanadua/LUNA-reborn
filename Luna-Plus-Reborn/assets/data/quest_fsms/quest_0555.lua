--[[
  Quest 555: Quest 555
  Level Required: 1
  NPC Start: 11, NPC Complete: 11
  Prerequisites: [554]
]]

local fsm = require('fsm_engine')

local quest_555 = fsm:new({
    id = 555,
    name = "Quest 555",
    level_required = 1,
    npc_start = 11,
    npc_complete = 11,
    prerequisites = {554},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 555: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 555: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 555: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 392,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 186,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 246,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 247,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 248,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 555: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 555: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 555: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 555: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_555