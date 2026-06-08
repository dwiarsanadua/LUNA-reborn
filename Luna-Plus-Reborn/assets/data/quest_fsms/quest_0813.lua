--[[
  Quest 813: [Party] Leader of the Trolls
  Level Required: 71
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=976642, Gold=23174
]]

local fsm = require('fsm_engine')

local quest_813 = fsm:new({
    id = 813,
    name = "[Party] Leader of the Trolls",
    level_required = 71,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 813: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 813: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 813: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 125,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 813: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 813: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 813: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 813: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 813: Rewards given")
                self:give_rewards({
                    exp = 976642,
                    gold = 23174,
                    items = {
                        { item_id = 21000009, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_813