--[[
  Quest 443: [Party][Quest] The Frightening One Eye
  Level Required: 53
  NPC Start: 119, NPC Complete: 119
  Prerequisites: [442]
  Rewards: EXP=288781, Gold=204165
]]

local fsm = require('fsm_engine')

local quest_443 = fsm:new({
    id = 443,
    name = "[Party][Quest] The Frightening One Eye",
    level_required = 53,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {442},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 443: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 443: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 443: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 443: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 443: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 443: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 443: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 443: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 443: Rewards given")
                self:give_rewards({
                    exp = 288781,
                    gold = 204165,
                })
            end,
            transitions = {},
        },
    },
})

return quest_443